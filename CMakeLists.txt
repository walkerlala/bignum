cmake_minimum_required (VERSION 3.16)
project (bignum VERSION 1.0)

include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)
include(CheckCXXCompilerFlag) # Check if the compiler supports the C++20 standard

check_cxx_compiler_flag("-std=c++20" COMPILER_SUPPORTS_CXX20)
check_cxx_compiler_flag("-std=c++17" COMPILER_SUPPORTS_CXX17)
check_cxx_compiler_flag("-std=c++11" COMPILER_SUPPORTS_CXX11)

message(STATUS "Compiler ID ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Compiler Version: ${CMAKE_CXX_COMPILER_VERSION}")
if(COMPILER_SUPPORTS_CXX20)
    message(STATUS "Compiler support C++20. Setting C++ standard to c++20")
    set(CMAKE_CXX_STANDARD 20)
#elseif(COMPILER_SUPPORTS_CXX17)
#    message(STATUS "Compiler support C++17. Setting C++ standard to c++17")
#    set(CMAKE_CXX_STANDARD 17)
#elseif(COMPILER_SUPPORTS_CXX11)
#    message(STATUS "Compiler support C++11. Setting C++ standard to c++11")
#    set(CMAKE_CXX_STANDARD 11)
else()
    message(FATAL_ERROR "Currently only C++20 is supported")
endif()

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

option(BIGNUM_WITH_ASAN "Building with address sanitizer" OFF)
option(BIGNUM_WITH_COVERAGE "Building with coverage" OFF)
option(BIGNUM_BUILD_TESTS "Build tests" OFF)
option(BIGNUM_BUILD_BENCHMARK "Build tests" OFF)
option(BIGNUM_BUILD_SHARED "Build shared library" OFF)

option(BIGNUM_ENABLE_EXCEPTIONS "Enable exceptions" ON)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-error=unused-parameter")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=ambiguous-reversed-operator")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=cast-function-type -Wno-error=implicit-fallthrough")
endif()

if(BIGNUM_ENABLE_EXCEPTIONS)
    add_compile_definitions(BIGNUM_ENABLE_EXCEPTIONS)
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")
endif()

if (BIGNUM_WITH_COVERAGE)
    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-instr-generate -fcoverage-mapping")
    else()
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif()
endif()

if(NOT DEFINED CMAKE_CXX_COMPILER_ID)
  message(FATAL_ERROR "CMAKE_CXX_COMPILER_ID not defined yet")
endif()
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

if(BIGNUM_WITH_ASAN)
  message(STATUS "Building with ASAN")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize-recover=address")
  if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libsan -Wno-uninitialized ")
  else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libasan -Wno-maybe-uninitialized ")
  endif()
endif()

include(cmake/gmp.cmake)
include(cmake/gtest.cmake)
include(cmake/benchmark.cmake)
find_package(Threads REQUIRED)

set(BIGNUM_SOURCE ${PROJECT_ROOT}/src/decimal.cc)
if (BIGNUM_BUILD_SHARED)
    add_library(bignum SHARED ${BIGNUM_SOURCE})
    target_include_directories(bignum PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(bignum PRIVATE ${GMP_INCLUDE_DIR})
    set_target_properties(bignum PROPERTIES SOVERSION 1 VERSION 1.0.0)
    add_dependencies(bignum gmp_shared_lib)
    target_link_libraries(bignum PUBLIC ${GMP_SHARED_LIBRARIES} Threads::Threads)
    set_target_properties(bignum PROPERTIES BUILD_RPATH "${GMP_LIB_DIR}")
else()
    add_library(bignum STATIC ${BIGNUM_SOURCE})
    target_include_directories(bignum PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(bignum PRIVATE ${GMP_INCLUDE_DIR} Threads::Threads)
    add_dependencies(bignum gmp_static_lib)
    target_link_libraries(bignum PUBLIC ${GMP_LIBRARIES})
endif()

# static bignum lib that use gmp only. For dev and test purpose. So build static lib only.
add_library(bignum_gmp_only STATIC ${BIGNUM_SOURCE})
target_include_directories(bignum_gmp_only PRIVATE ${PROJECT_ROOT}/src)
target_include_directories(bignum_gmp_only PRIVATE ${GMP_INCLUDE_DIR} Threads::Threads)
target_compile_definitions(bignum_gmp_only PUBLIC BIGNUM_DEV_USE_GMP_ONLY)
add_dependencies(bignum_gmp_only gmp_static_lib)
target_link_libraries(bignum_gmp_only PRIVATE ${GMP_LIBRARIES})

# decimal_calculator
add_executable(decimal_calculator ${CMAKE_SOURCE_DIR}/src/calculator.cc)
target_link_libraries(decimal_calculator bignum)
target_include_directories(decimal_calculator PRIVATE ${PROJECT_ROOT}/src)
target_include_directories(decimal_calculator PRIVATE ${GMP_INCLUDE_DIR})

# decimal_calculator_gmp_only
add_executable(decimal_calculator_gmp_only ${CMAKE_SOURCE_DIR}/src/calculator.cc)
target_link_libraries(decimal_calculator_gmp_only bignum_gmp_only)
target_include_directories(decimal_calculator_gmp_only PRIVATE ${PROJECT_ROOT}/src)
target_include_directories(decimal_calculator_gmp_only PRIVATE ${GMP_INCLUDE_DIR})

if (BIGNUM_BUILD_TESTS)
    set(UNITTEST_SOURCES
        ${PROJECT_ROOT}/tests/main.cc
        ${PROJECT_ROOT}/tests/op.cc
        ${PROJECT_ROOT}/tests/gmp.cc
        ${PROJECT_ROOT}/tests/issues.cc
        ${PROJECT_ROOT}/tests/exception_or_assert.cc
    )
    add_executable(unittest ${UNITTEST_SOURCES})
    target_link_libraries(unittest bignum)
    target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(unittest PRIVATE ${GMP_INCLUDE_DIR})
    target_include_directories(unittest PRIVATE ${GTEST_INCLUDE_DIR})
    target_link_libraries(unittest ${GTEST_LIBRARIES} Threads::Threads)
    add_dependencies(unittest gtest_lib)

    add_executable(unittest_gmp_only ${UNITTEST_SOURCES})
    target_link_libraries(unittest_gmp_only bignum_gmp_only)
    target_include_directories(unittest_gmp_only PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(unittest_gmp_only PRIVATE ${GMP_INCLUDE_DIR})
    target_include_directories(unittest_gmp_only PRIVATE ${GTEST_INCLUDE_DIR})
    target_compile_definitions(unittest_gmp_only PUBLIC BIGNUM_DEV_USE_GMP_ONLY)
    target_link_libraries(unittest_gmp_only ${GTEST_LIBRARIES} Threads::Threads)
    add_dependencies(unittest_gmp_only gtest_lib)
endif()

if (BIGNUM_BUILD_BENCHMARK)
    SET(BENCHMARK_SOURCES
        ${PROJECT_ROOT}/benchmark/main.cc
        ${PROJECT_ROOT}/benchmark/op.cc
    )
    add_executable(benchmark ${BENCHMARK_SOURCES})
    target_link_libraries(benchmark bignum)
    target_include_directories(benchmark PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(benchmark PRIVATE ${GMP_INCLUDE_DIR})
    target_include_directories(benchmark PRIVATE ${BENCHMARK_INCLUDE_DIR})
    target_link_libraries(benchmark ${BENCHMARK_LIBRARIES} Threads::Threads)
    add_dependencies(benchmark benchmark_lib)
endif()

set_target_properties(
    bignum
    PROPERTIES
    PUBLIC_HEADER
    "${PROJECT_ROOT}/src/assertion.h;${PROJECT_ROOT}/src/decimal.h;${PROJECT_ROOT}/src/errcode.h;${PROJECT_ROOT}/src/gmp_wrapper.h"
)
set_target_properties(
    bignum
    PROPERTIES
    INSTALL_RPATH "\$ORIGIN"
)
install(TARGETS bignum
        ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_PREFIX}/include/bignum
)
if (BIGNUM_BUILD_SHARED)
    install(FILES ${GMP_LIB_DIR}/libgmp.so DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    install(FILES ${GMP_LIB_DIR}/libgmp.so.10 DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
    install(FILES ${GMP_LIB_DIR}/libgmp.so.10.5.0 DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
endif()

install(TARGETS decimal_calculator ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
install(TARGETS decimal_calculator_gmp_only ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
if (BIGNUM_BUILD_TESTS)
    install(TARGETS unittest RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
    install(TARGETS unittest_gmp_only RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()
if (BIGNUM_BUILD_BENCHMARK)
    install(TARGETS benchmark RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
endif()
