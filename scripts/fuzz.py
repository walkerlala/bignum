#!/usr/bin/env python3
# coding:utf-8

from decimal import Decimal
from mysql.connector import Error
from tqdm import tqdm
import argparse
import mysql.connector
import os
import random
import string
import subprocess
import pdb

parser = argparse.ArgumentParser()
parser.add_argument("--test_count", dest="test_count", type=int)
parser.add_argument("--decimal_calculator", dest="decimal_calculator", type=str)
parser.add_argument("--mysql_host", dest="mysql_host", type=str)
parser.add_argument("--mysql_port", dest="mysql_port", type=int)
parser.add_argument("--mysql_user", dest="mysql_user", type=str)
parser.add_argument("--mysql_password", dest="mysql_password", type=str)
args = parser.parse_args()

max_decimal_precision = 65
max_decimal_scale = 30
special_values = [
    Decimal("99999999999999999999999999999999999999999999999999999999999999999"),  # 65-digits max
    Decimal("-99999999999999999999999999999999999999999999999999999999999999999"),  # 65-digits min
    Decimal("0"),
    Decimal("1"),
    Decimal("-1"),
    Decimal("-9223372036854775808"),  # INT64_MIN
    Decimal("9223372036854775807"),  # INT64_MAX
    Decimal("-170141183460469231731687303715884105728"),  # INT128_MIN
    Decimal("170141183460469231731687303715884105727"),  # INT128_MAX
    # random decimal point within these values
    Decimal("999999999999999.99999999999999999999999999999999999999999999999999"),
    Decimal("-999999999999999999.99999999999999999999999999999999999999999999999"),
    Decimal("0"),
    Decimal("1"),
    Decimal("-1"),
    Decimal("-922.3372036854775808"),
    Decimal("922337.2036854775807"),
    Decimal("-170141.183460469231731687303715884105728"),
    Decimal("170141183460.469231731687303715884105727"),
    # random decimal point within these values
    Decimal("99999999999999999999999999999999999999999999999999999999999.999999"),
    Decimal("-999999999999999999999999999999999999999999999999999999.99999999999"),
    Decimal("0"),
    Decimal("1"),
    Decimal("-1"),
    Decimal("-9223372036854775.808"),
    Decimal("9223372036.854775807"),
    Decimal("-1701411834604692.31731687303715884105728"),
    Decimal("1701411834604692317316.87303715884105727"),
    # random decimal point within these values
    Decimal("99999999999999999999999999999999999999999999999999999999999.999999000"),
    Decimal("-999999999999999999999999999999999999999999999999999999.9999999999900"),
    Decimal("0"),
    Decimal("1"),
    Decimal("-1"),
    Decimal("-9223372036854775.808000"),
    Decimal("9223372036.854775807000"),
    Decimal("-1701411834604692.31731687303715884105728000"),
    Decimal("1701411834604692317316.87303715884105727000"),
]


try:
    mysql_conn = mysql.connector.connect(
        host=args.mysql_host,
        port=args.mysql_port,
        user=args.mysql_user,
        password=args.mysql_password,
    )
    if not mysql_conn.is_connected():
        raise Exception("Failed to connect to mysql")

    db_info = mysql_conn.get_server_info()
    print("Connected to MySQL Server version ", db_info)

    mysql_cursor = mysql_conn.cursor()
except Error as e:
    raise Exception(f"Error while connecting to MySQL: {str(e)}")


def get_random_string(length):
    assert (
        length <= max_decimal_precision
    ), f"random string too long: {length} (max {max_decimal_precision})"
    letters = string.digits
    result_str = "".join(random.choice(letters) for i in range(length))
    result_str = result_str.lstrip("0")
    return result_str


def get_decimal_scale(s: str) -> int:
    lst = s.split(".")
    if len(lst) == 1:
        return 0
    elif len(lst) == 2:
        return len(lst[1])
    else:
        assert False, f"Invalid decimal str {s}"


def run_mysql_test(d1: Decimal, d2: Decimal, op: str) -> str:
    try:
        d1str = str(d1)
        d1scale = get_decimal_scale(d1str)

        d2str = str(d2)
        d2scale = get_decimal_scale(d2str)

        sql = f""" SELECT CAST("{d1str}" AS DECIMAL(65, {d1scale}))  {op}  CAST("{d2str}" AS DECIMAL(65, {d2scale})) ; """
        mysql_cursor.execute(sql)
        record = mysql_cursor.fetchone()
        return str(record[0]), sql

    except Exception as e:
        raise Exception(f"mysql query err: {str(e)}")


def run_bignum_test(d1: Decimal, d2: Decimal, op: str) -> str:
    result = subprocess.run(
        [args.decimal_calculator, str(d1), str(d2), op],
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
    )
    if len(result.stderr):
        raise Exception(f"decimal_calculator err: {result.stderr}")
    elif not len(result.stdout):
        raise Exception(f"decimal_calculator err: empty result")

    return result.stdout.decode()

def decimal_str_cmp(dmysql: str, dbignum: str) -> bool:
    if dmysql == dbignum:
        return True

    # bignum trim trailing '0' in least significant part
    # but mysql does not. So we should treat 1.23400 == 1.234
    if '.' in dbignum and dmysql.startswith(dmysql) and dmysql.lstrip(dmysql).strip("0") == ".":
        return True

    return False

def test_binary_expr(d1: Decimal, d2: Decimal, op: str):
    mysql_e = None
    bignum_e = None
    dmysql = ""
    dbignum = ""

    try:
        dmysql, mysql_sql = run_mysql_test(d1, d2, op)
    except Exception as e:
        mysql_e = e
        pass

    try:
        dbignum = run_bignum_test(d1, d2, op)
    except Exception as e:
        bignum_e = e
        pass

    if (mysql_e is None) != (bignum_e is None):
        if mysql_e is not None:
            print(
                f"\nmysql exception but bignum succeed for {d1} {op} {d2}: {str(mysql_e)}, mysql_sql: {mysql_sql}\n"
            )
            return False
        else:
            print(
                f"\nbignum exception but mysql succeed for {d1} {op} {d2}: {str(bignum_e)}, mysql_sql: {mysql_sql}\n"
            )
            return False

    res = decimal_str_cmp(dmysql, dbignum)
    if not res:
        print(
            f"\nResult mismatch: {d1} {op} {d2}, expect {dmysql}, actual {dbignum}, mysql_sql: {mysql_sql}\n"
        )
        return False
    else:
        return True


def get_random_decimal_from_digits_str(s: str) -> Decimal:
    l = len(s)
    rv = random.randint(0, 10)
    if l < 1:
        return Decimal("0")
    elif l == 1:
        if rv <= 5:  # place a decimal point before these digits
            return Decimal(f"0.{s}")
        else:
            return Decimal("-1")
    else:
        least_significant_len = random.randint(0, max(max_decimal_scale, l))
        most_significant_len = l - least_significant_len

        least_significant_part = s[0:least_significant_len]
        most_significant_part = s[least_significant_len:]

        if not least_significant_len:
            return Decimal(most_significant_part)
        else:
            return Decimal(f"{most_significant_part}.{least_significant_part}")


def test_exprs(num_random_numbers):
    num_tests = num_random_numbers
    succ_cnt = 0
    fail_cnt = 0
    for _ in tqdm(range(num_random_numbers)):
        val1 = random.randint(0, max_decimal_precision)
        str1 = get_random_string(val1)
        if not len(str1):
            continue

        val2 = random.randint(0, max_decimal_precision)
        str2 = get_random_string(val2)
        if not len(str2):
            continue

        d1 = get_random_decimal_from_digits_str(str1)
        d2 = get_random_decimal_from_digits_str(str2)

        r = random.randint(0, 125)
        if r >= 0 and r < 25:
            res = test_binary_expr(d1, d2, "+")
        elif r >= 25 and r < 50:
            res = test_binary_expr(d1, d2, "-")
        elif r >= 50 and r < 75:
            res = test_binary_expr(d1, d2, "*")
        elif r >= 75 and r < 100:
            res = test_binary_expr(d1, d2, "/")
        elif r >= 100 and r < 125:
            res = test_binary_expr(d1, d2, "%")
        else:
            assert False

        if res:
            succ_cnt += 1
        else:
            fail_cnt += 1

    print(f"\n\tTotal expr tested {num_tests}, success {succ_cnt}, failed {fail_cnt}\n")


def main():
    if not os.path.exists(args.decimal_calculator):
        raise Exception(f"decimal_calculator not found: {args.decimal_calculator}")

    test_exprs(args.test_count)


if __name__ == "__main__":
    main()
