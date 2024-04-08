#!/bin/bash

# Directory of current script
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
PROJECT_ROOT=$(dirname $SCRIPT_DIR)

# If there is parameter, use it as the test_count
if [ $# -eq 1 ]; then
    TEST_COUNT=$1
else
    TEST_COUNT=1000000
fi

# Check that "${PROJECT_ROOT}/venv" exists
if [ ! -d "${PROJECT_ROOT}/venv" ]; then
    echo "Virtual environment not found. Please create with `python3 -m venv venv` in project directory first."
    exit 1
fi

# Check that "${PROJECT_ROOT}/install/bin/decimal_calculator" exists, to make sure that this project
# is built
if [ ! -f "${PROJECT_ROOT}/install/bin/decimal_calculator" ]; then
    echo "Project not built. Please build with `./ibuild.sh` first."
    exit 1
fi

set -x
${PROJECT_ROOT}/venv/bin/python3 scripts/fuzz.py \
        --decimal_calculator ${PROJECT_ROOT}/install/bin/decimal_calculator \
        --pg_host 127.0.0.1 \
        --pg_port 5432 \
        --pg_user mypguser \
        --pg_password mypgpass \
        --pg_database mypgdb  \
        --test_count ${TEST_COUNT}

${PROJECT_ROOT}/venv/bin/python3 scripts/fuzz.py \
        --decimal_calculator ${PROJECT_ROOT}/install/bin/decimal_calculator_gmp_only \
        --pg_host 127.0.0.1 \
        --pg_port 5432 \
        --pg_user mypguser \
        --pg_password mypgpass \
        --pg_database mypgdb  \
        --test_count ${TEST_COUNT}
set +x
