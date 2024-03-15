cmake_minimum_required (VERSION 3.22)
project (bignum VERSION 1.0)

include(CheckCSourceCompiles)
include(CheckCXXSourceCompiles)

set(CMAKE_CXX_STANDARD 20)

set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

option(BIGNUM_WITH_ASAN "Building with address sanitizer" OFF)
option(BIGNUM_WITH_COVERAGE "Building with coverage" OFF)
option(BIGNUM_BUILD_TESTS "Build tests" OFF)
option(BIGNUM_BUILD_SHARED "Build shared library" OFF)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -Wno-error=unused-parameter")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=ambiguous-reversed-operator")
else()
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=cast-function-type -Wno-error=implicit-fallthrough")
endif()

if (WITH_COVERAGE)
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

if(WITH_ASAN)
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

set(BIGNUM_SOURCE ${PROJECT_ROOT}/src/Decimal.cc)
if (BIGNUM_BUILD_SHARED)
    add_library(bignum SHARED ${BIGNUM_SOURCE})
    target_include_directories(bignum PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(bignum PRIVATE ${GMP_INCLUDE_DIR})
    target_link_libraries(bignum gmp_shared)
    set_target_properties(bignum PROPERTIES SOVERSION 1 VERSION 1.0.0)
else()
    add_library(bignum STATIC ${BIGNUM_SOURCE})
    target_include_directories(bignum PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(bignum PRIVATE ${GMP_INCLUDE_DIR})
    target_link_libraries(bignum gmp_static)
endif()

install(TARGETS bignum
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

add_executable(test ${PROJECT_ROOT}/src/test.cc)
target_include_directories(test PRIVATE ${PROJECT_ROOT}/src)
target_include_directories(test PRIVATE ${GMP_INCLUDE_DIR})
target_link_libraries(test bignum gmp_static)

if (BIGNUM_BUILD_TESTS)
    add_executable(test_decimal ${PROJECT_ROOT}/src/test_decimal.cc)
    target_link_libraries(test_decimal bignum)
    install(TARGETS test_decimal
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )
endif()
