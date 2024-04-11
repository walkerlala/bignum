INCLUDE(ExternalProject)

MESSAGE(STATUS "============================================")
MESSAGE(STATUS "=          Building google benchmark       =")
MESSAGE(STATUS "============================================")

SET(BENCHMARK_VERSION 1.8.2)
SET(BENCHMARK_DIR         "${CMAKE_SOURCE_DIR}/extra/benchmark/")
SET(BENCHMARK_TAR_BALL    "${CMAKE_SOURCE_DIR}/extra/benchmark/v${BENCHMARK_VERSION}.tar.gz")
SET(BENCHMARK_SOURCE_DIR  "${CMAKE_SOURCE_DIR}/extra/benchmark/benchmark-${BENCHMARK_VERSION}/")
SET(BENCHMARK_BUILD_DIR   "${CMAKE_SOURCE_DIR}/extra/benchmark/benchmark-${BENCHMARK_VERSION}/build")
SET(BENCHMARK_INSTALL_DIR "${CMAKE_SOURCE_DIR}/extra/benchmark/benchmark-${BENCHMARK_VERSION}/install")

# Decompress source files
EXECUTE_PROCESS(
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_SOURCE_DIR}/extra/benchmark/v${BENCHMARK_VERSION}.tar.gz
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extra/benchmark/
    RESULT_VARIABLE tar_result
)
# create build dir
EXECUTE_PROCESS(
    COMMAND mkdir -p ${BENCHMARK_BUILD_DIR}
    WORKING_DIRECTORY ${BENCHMARK_DIR}
    RESULT_VARIABLE mkdir_result
)

SET(BENCHMARK_CMAKE
cmake ${BENCHMARK_SOURCE_DIR}
    -DCMAKE_BUILD_TYPE=Release
    -DBENCHMARK_ENABLE_TESTING=OFF
    -DCMAKE_INSTALL_PREFIX=${BENCHMARK_INSTALL_DIR}
)

ExternalProject_Add(benchmark_external
    SOURCE_DIR ${BENCHMARK_SOURCE_DIR}
    BINARY_DIR ${BENCHMARK_BUILD_DIR}
    STAMP_DIR  ${BENCHMARK_BUILD_DIR}
    CONFIGURE_COMMAND ${BENCHMARK_CMAKE}
    BUILD_COMMAND  make -j${DEFAULT_BUILD_JOBS}
    INSTALL_COMMAND make install -j${DEFAULT_BUILD_JOBS}
)

ADD_LIBRARY(benchmark_lib OBJECT IMPORTED)
SET_TARGET_PROPERTIES(
    benchmark_lib
    PROPERTIES
    IMPORTED_LOCATION "${BENCHMARK_INSTALL_DIR}/lib/libbenchmark.a"
)
ADD_DEPENDENCIES(benchmark_lib benchmark_external)

SET(BENCHMARK_INCLUDE_DIR "${BENCHMARK_INSTALL_DIR}/include" CACHE STRING "Path to benchmark include directory")
SET(BENCHMARK_LIBRARIES "${BENCHMARK_INSTALL_DIR}/lib/libbenchmark.a" CACHE STRING "Path to benchmark library")
SET(BENCHMARK_MAIN_LIBRARIES "${BENCHMARK_INSTALL_DIR}/lib/libbenchmark.a" CACHE STRING "benchmark main")
