#!/bin/bash

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
echo "Directory of current script: $SCRIPT_DIR"
cd ${SCRIPT_DIR}

root_dir=`pwd`

build_type="release"
inst_dir="${root_dir}/install"
san_type=""
compiler="gcc"
coverage=0
asan=0
tsan=0

get_key_value()
{
  echo "$1" | sed 's/^-[a-zA-Z_-]*=//'
}

usage()
{
cat <<EOF
Usage: $0 [-t debug|release] [-d <inst_dir>]
       Or
       $0 [-h | --help]
  -t   Select the build type: release, debug.
  -d   Set the destination directory.
  -g   Enable the sanitizer of compiler,
       asan for AddressSanitizer, tsan for ThreadSanitizer
  -h, --help              Show this help message.
  -m   Select compiler: gcc, clang
  -a   Enable coverage build
EOF
}

parse_options()
{
  while test $# -gt 0
  do
    case "$1" in
    -t=*)
      build_type=`get_key_value "$1"`;;
    -t)
      shift
      build_type=`get_key_value "$1"`;;
    -d=*)
      inst_dir=`get_key_value "$1"`;;
    -d)
      shift
      inst_dir=`get_key_value "$1"`;;
    -g=*)
      san_type=`get_key_value "$1"`;;
    -g)
      shift
      san_type=`get_key_value "$1"`;;
    -m=*)
      compiler=`get_key_value "$1"`;;
    -m)
      shift
      compiler=`get_key_value "$1"`;;
    -a)
      coverage=1;;
    -h | --help)
      usage
      exit 0;;
    *)
      echo "Unknown option '$1'"
      exit 1;;
    esac
    shift
  done
}

dump_options()
{
  echo "=-------- Dumping the options used by $0 ------------="
  echo "c_compiler: $c_compiler"
  echo "cxx_compiler: $cxx_compiler"
  echo "build_type=$build_type"
  echo "Sanitizer=$san_type"
  echo "coverage=$coverage"
  echo ""
}

parse_options "$@"
dump_options

if [ x"$build_type" = x"debug" ]; then
  build_type="Debug"
elif [ x"$build_type" = x"release" ]; then
  build_type="RelWithDebInfo"
else
  echo "Invalid build type, it must be \"debug\", \"release\" ."
  exit 1
fi

if [ x"$san_type" = x"" ]; then
    asan=0
    tsan=0
elif [ x"$san_type" = x"asan" ]; then
    asan=1
    tsan=0
elif [ x"$san_type" = x"tsan" ]; then
    asan=0
    tsan=1
else
  echo "Invalid sanitizer type, it must be \"asan\" or \"tsan\"."
  exit 1
fi

if [ x"$compiler" = x"gcc" ]; then
  c_compiler="gcc"
  cxx_compiler="g++"
elif [ x"$compiler" = x"clang" ]; then
  c_compiler="clang"
  cxx_compiler="clang++"
else
  echo "Invalid build type, it must be \"gcc\", \"clang\" ."
  exit 1
fi

default_build_dir=${root_dir}/build/${build_type}
mkdir -p $default_build_dir && cd $default_build_dir

cmake \
  -DCMAKE_BUILD_TYPE="$build_type"      \
  -DCMAKE_INSTALL_PREFIX="$inst_dir"    \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=1     \
  -DCMAKE_C_COMPILER="$c_compiler"      \
  -DCMAKE_CXX_COMPILER="$cxx_compiler"  \
  -DWITH_COVERAGE="$coverage"           \
  -DWITH_ASAN=$asan                     \
  -DBIGNUM_BUILD_TESTS=1                \
  ${root_dir}

cp ${default_build_dir}/compile_commands.json ${root_dir}

make install -j8 VERBOSE=1
