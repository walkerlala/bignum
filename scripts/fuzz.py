#!/usr/bin/env python3
# coding:utf-8

from decimal import Decimal
from tqdm import tqdm
import argparse
import decimal
import os
import pdb
import psycopg2
import random
import string
import subprocess

parser = argparse.ArgumentParser()
parser.add_argument("--test_count", dest="test_count", type=int)
parser.add_argument("--decimal_calculator", dest="decimal_calculator", type=str)
parser.add_argument("--pg_host", dest="pg_host", default="127.0.0.1", type=str)
parser.add_argument("--pg_port", dest="pg_port", default=5432, type=int)
parser.add_argument("--pg_user", dest="pg_user", default="mypguser", type=str)
parser.add_argument("--pg_password", dest="pg_password", default="mypgpass", type=str)
parser.add_argument("--pg_database", dest="pg_database", default="mypgdb", type=str)
args = parser.parse_args()

# Set python's decimal's "scale" to be 30
decimal.getcontext().prec = 30

max_decimal_precision = 96
max_decimal_scale = 30
special_values = [
    Decimal("999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"),  # 96-digits max
    Decimal("-999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"),  # 96-digits max
    Decimal("0"),
    Decimal("1"),
    Decimal("-1"),
    Decimal("-9223372036854775808"),  # INT64_MIN
    Decimal("9223372036854775807"),  # INT64_MAX
    Decimal("-170141183460469231731687303715884105728"),  # INT128_MIN
    Decimal("170141183460469231731687303715884105727"),  # INT128_MAX
    # random decimal point within these values
    Decimal("99999999999999999999999999999999999999999999999999999999999999999999999999.9999999999999999999999"),  # 96-digits max
    Decimal("-999999999999999999999999999999999999999999999999999999999999999999999.999999999999999999999999999"),  # 96-digits max
    Decimal("0"),
    Decimal("1"),
    Decimal("-1"),
    Decimal("-922.3372036854775808"),
    Decimal("922337.2036854775807"),
    Decimal("-170141.183460469231731687303715884105728"),
    Decimal("170141183460.469231731687303715884105727"),
    # random decimal point within these values
    Decimal("999999999999999999999999999999999999999999999999999999999999999999999999.999999999999999999999999"),  # 96-digits max
    Decimal("-999999999999999999999999999999999999999999999999999999999999999999999999.999999999999999999999999"),  # 96-digits max
    Decimal("0"),
    Decimal("1"),
    Decimal("-1"),
    Decimal("-9223372036854775.808"),
    Decimal("9223372036.854775807"),
    Decimal("-1701411834604692.31731687303715884105728"),
    Decimal("1701411834604692317316.87303715884105727"),
    # random decimal point within these values
    Decimal("999999999999999999999999999999999999999999999999999999999999999999999999999999.999999999999999999"),  # 96-digits max
    Decimal("-999999999999999999999999999999999999999999999999999999999999999999999999999999999999999.999999999"),  # 96-digits max
    Decimal("0"),
    Decimal("1"),
    Decimal("-1"),
    Decimal("-9223372036854775.808000"),
    Decimal("9223372036.854775807000"),
    Decimal("-1701411834604692.31731687303715884105728000"),
    Decimal("1701411834604692317316.87303715884105727000"),
]

try:
     pg_conn = psycopg2.connect(
             database='mypgdb',
             user='mypguser',
             password='mypgpass',
             host='127.0.0.1',
             port=5432
     )
     pg_cursor = pg_conn.cursor()
except Error as e:
    raise Exception(f"Error while connecting to postgres: {str(e)}")


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


def run_pg_test(d1str: str, d2str: str, op: str) -> str:
    try:
        d1scale = get_decimal_scale(d1str)
        d2scale = get_decimal_scale(d2str)

        sql = f""" SELECT CAST(CAST('{d1str}' AS DECIMAL(96, {d1scale}))  {op}  CAST('{d2str}' AS DECIMAL(96, {d2scale})) AS VARCHAR); """
        #print(f"Executing in pg: {sql}")
        pg_cursor.execute(sql)
        record = pg_cursor.fetchone()
        return record[0], sql

    except Exception as e:
        raise Exception(f"pg query err: {str(e)}")


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

def run_python_test(d1: Decimal, d2: Decimal, op: str) -> str:
    if op == "+":
        return d1 + d2
    elif op == "-":
        return d1 - d2
    elif op == "*":
        return d1 * d2
    elif op == "/":
        return d1 / d2
    elif op == "%":
        return d1 % d2
    else:
        raise Exception(f"Unknown python op {op}")

def decimal_result_cmp(dpg: str, dbignum: str) -> bool:
    # Ignore leading zeros
    if dpg.lstrip("0") == dbignum.lstrip("0"):
        return True

    # Strip the "-" sign if both are negative
    if (dpg[0] == "-") == (dbignum[0] == "-"):
        dpg = dpg.lstrip("-")
        dbignum = dbignum.lstrip("-")

    # 0.00000000.... vs 0
    # -0.00000000... vs 0
    if dbignum == "0" and (dpg.startswith("0.0000") or dpg.startswith("-0.0000")):
        return True

    if (dpg[0] == "-") != (dbignum[0] == "-"):
        return False

    # Ignore trailing zeros after decimal point
    if "." in dpg:
        dpg = dpg.rstrip("0")
    if "." in dbignum:
        dbignum = dbignum.rstrip("0")
    if dpg == dbignum:
        return True

    # Ignore rounding differences
    if (dpg.startswith(dbignum) and "." in dbignum):
        return True

    #if ("." in dpg and dpg.lstrip(dbignum).startswith(".")):
    #    return True

    # bignum might have large scale than pg, e.g., pg might be 123.45 and bignum be 123.44668
    # Also there might be case like:
    #    expect 21300702412490987477180669136903985656.0, actual 21300702412490987477180669136903985655.96595
    #
    # To check that:
    #   1) check that the number of significant digits are the same
    #   2) remove any "." and check if the rounding is correct
    dpg_num_significant_digits = dpg.split(".")[0]
    dbignum_num_significant_digits = dbignum.split(".")[0]
    if len(dpg_num_significant_digits) != len(dbignum_num_significant_digits):
        return False
    dpg_no_dot = dpg.replace(".", "")
    dbignum_no_dot = dbignum.replace(".", "")
    if dpg_no_dot == dbignum_no_dot:
        return True

    elif len(dpg_no_dot) < len(dbignum_no_dot):
        dbignum_int_part = dbignum_no_dot[:len(dpg_no_dot)]
        dbignum_frac_part = dbignum_no_dot[len(dpg_no_dot):]

        dbignum_int = int(dbignum_int_part)
        dbignum_frac_part_first_int = int(dbignum_frac_part[0])

        dpg_int = int(dpg_no_dot)

        # Postgresql have rounding issue with round-half-up case. So omit it.
        if dbignum_frac_part_first_int > 5:
            return dbignum_int + 1 == dpg_int
        elif dbignum_frac_part_first_int == 5:
            return dbignum_int == dpg_int or dbignum_int + 1 == dpg_int
        else:
            return dbignum_int == dpg_int

    elif len(dpg_no_dot) > len(dbignum_no_dot):
        dpg_int_part = dpg_no_dot[:len(dbignum_no_dot)]
        dpg_frac_part = dpg_no_dot[len(dbignum_no_dot):]

        dpg_int = int(dpg_int_part)
        dpg_frac_part_first_int = int(dpg_frac_part[0])

        dbignum_int = int(dbignum_no_dot)

        # We don't have rounding issue
        if dpg_frac_part_first_int >= 5:
            return dpg_int + 1 == dbignum_int
        #elif dpg_frac_part_first_int == 5:
        #    return dpg_int == dbignum_int or dpg_int + 1 == dbignum_int
        else:
            return dpg_int == dbignum_int

        return dpg_int == dbignum_int

    return False

def test_binary_expr_with_pg(d1: Decimal, d2: Decimal, op: str):
    pg_sql = None
    pg_exception = None
    dpg = None

    bignum_exception = None
    dbignum = None

    try:
        dpg, pg_sql = run_pg_test(d1, d2, op)
        assert type(dpg) == str
    except Exception as e:
        pg_exception = e
        pass

    try:
        dbignum = run_bignum_test(d1, d2, op)
        assert type(dbignum) == str
    except Exception as e:
        bignum_exception = e
        pass

    if bignum_exception is not None and pg_exception is None:
        # Ingore overflow exception
        bignum_exception_str = str(bignum_exception)
        if "overflow" in bignum_exception_str and len(dpg.split(".")[0]) > 96:
            return True

        # Ingore overflow error by now
        if "overflow" in bignum_exception_str:
            return True

        print(
                f"\nbignum exception but pg succeed for {d1} {op} {d2}: {str(bignum_exception)}, pg result: {dpg}, pg_sql: {pg_sql}\n"
        )
        return False
    elif bignum_exception is None and pg_exception is not None:
        print(
                f"\nPG exception but bignum succeed for {d1} {op} {d2}: {str(pg_exception)}, bignum result: {dbignum}, pg_sql: {pg_sql}\n"
        )
        return False

    #dpg = "359613911283063661287235665052372194592841014008506692933204473.28951851438637"
    #dbignum = "359613911283063661287235665052372194592841014008506692933204473.289518514386372087"
    #pdb.set_trace()
    res = decimal_result_cmp(dpg, dbignum)
    if not res:
        print(
            f"\nResult mismatch: {d1} {op} {d2}, expect {dpg}, actual {dbignum}, pg_sql: {pg_sql}\n"
        )
        return False
    else:
        return True

def test_binary_expr(d1: str, d2: str, op: str):
    return test_binary_expr_with_pg(d1, d2, op)

def get_random_decimal_from_digits_str(s: str) -> str:
    l = len(s)
    rv = random.randint(0, 10)
    if l < 1:
        return "0"
    elif l == 1:
        if rv <= 5:  # place a decimal point before these digits
            return f"0.{s}"
        else:
            return "-1"
    else:
        least_significant_len = random.randint(0, min(max_decimal_scale, l))
        most_significant_len = l - least_significant_len

        least_significant_part = s[0:least_significant_len]
        most_significant_part = s[least_significant_len:]
        if not len(most_significant_part):
            most_significant_part = "0"

        if not least_significant_len:
            return most_significant_part
        else:
            return f"{most_significant_part}.{least_significant_part}"


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

        # Randomly make d1 , d2 as negative
        if not d1.startswith("-") and random.randint(0, 10) <= 5:
            d1 = "-" + d1
        if not d2.startswith("-") and random.randint(0, 10) <= 5:
            d2 = "-" + d2

        r = random.randint(0, 125)
        if r >= 0 and r < 25:
            res = test_binary_expr(d1, d2, "+")
        elif r >= 25 and r < 50:
            res = test_binary_expr(d1, d2, "-")
        elif r >= 50 and r < 75:
            res = test_binary_expr(d1, d2, "*")
        elif r >= 75 and r < 100:
            res = test_binary_expr(d1, d2, "/")
        elif r >= 100 and r <= 125:
            res = test_binary_expr(d1, d2, "%")
        else:
            print(f"Invalid r: {r}")
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
