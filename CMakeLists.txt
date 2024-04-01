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
elseif(COMPILER_SUPPORTS_CXX17)
    message(STATUS "Compiler support C++17. Setting C++ standard to c++17")
    set(CMAKE_CXX_STANDARD 17)
elseif(COMPILER_SUPPORTS_CXX11)
    message(STATUS "Compiler support C++11. Setting C++ standard to c++11")
    set(CMAKE_CXX_STANDARD 11)
else()
    message(FATAL_ERROR "Compiler should at least support C++11")
endif()

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
find_package(Threads REQUIRED)

# TODO set install rpath for gmp if needed
set(BIGNUM_SOURCE ${PROJECT_ROOT}/src/Decimal.cc)
if (BIGNUM_BUILD_SHARED)
    add_library(bignum SHARED ${BIGNUM_SOURCE})
    target_include_directories(bignum PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(bignum PRIVATE ${GMP_INCLUDE_DIR})
    set_target_properties(bignum PROPERTIES SOVERSION 1 VERSION 1.0.0)
    add_dependencies(bignum gmp_shared_lib)
    target_link_libraries(bignum PRIVATE ${GMP_SHARED_LIBRARIES} Threads::Threads)
else()
    add_library(bignum STATIC ${BIGNUM_SOURCE})
    target_include_directories(bignum PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(bignum PRIVATE ${GMP_INCLUDE_DIR} Threads::Threads)
    add_dependencies(bignum gmp_static_lib)
    target_link_libraries(bignum PRIVATE ${GMP_LIBRARIES})
endif()

install(TARGETS bignum
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

if (BIGNUM_BUILD_TESTS)
    add_executable(unittest ${PROJECT_ROOT}/tests/op.cc)
    target_link_libraries(unittest bignum)
    target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)
    target_include_directories(unittest PRIVATE ${GMP_INCLUDE_DIR})
    target_include_directories(unittest PRIVATE ${GTEST_INCLUDE_DIR})
    target_link_libraries(unittest ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES} Threads::Threads)
    add_dependencies(unittest gtest_lib)
    install(TARGETS unittest
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        INCLUDES DESTINATION include
    )
endif()

################################################################################
# These binaries and libraries are for dev purposes only
################################################################################
#add_executable(test ${PROJECT_ROOT}/scripts/test.cc)
#target_include_directories(test PRIVATE ${PROJECT_ROOT}/src)
#target_include_directories(test PRIVATE ${GMP_INCLUDE_DIR})
#target_link_libraries(test bignum)
#
#add_executable(GenerateGmpPow10 ${PROJECT_ROOT}/scripts/GenerateGmpPow10.cc)
#target_include_directories(GenerateGmpPow10 PRIVATE ${GMP_INCLUDE_DIR})
#target_link_libraries(GenerateGmpPow10 ${GMP_LIBRARIES})
