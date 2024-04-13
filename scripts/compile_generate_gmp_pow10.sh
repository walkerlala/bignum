#!/bin/bash

# Directory of current script
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd ${SCRIPT_DIR}

set -x
g++ -Wall -std=c++20 \
        -I/home/walkerlala/projects/tmp/gmp-6.3.0/install/include/ \
        -L/home/walkerlala/projects/tmp/gmp-6.3.0/install/lib/ \
        ./generate_gmp_pow10.cc -o generate_gmp_pow10 -lgmp

./generate_gmp_pow10

set +x
