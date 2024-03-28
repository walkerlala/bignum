INCLUDE(ExternalProject)

MESSAGE(STATUS "============================================")
MESSAGE(STATUS "=           Building gtest                 =")
MESSAGE(STATUS "============================================")

SET(GTEST_VERSION 1.14.0)
SET(GTEST_DIR         "${CMAKE_SOURCE_DIR}/extra/gtest/")
SET(GTEST_TAR_BALL    "${CMAKE_SOURCE_DIR}/extra/gtest/v${GTEST_VERSION}.tar.gz")
SET(GTEST_SOURCE_DIR  "${CMAKE_SOURCE_DIR}/extra/gtest/googletest-${GTEST_VERSION}/")
SET(GTEST_BUILD_DIR   "${CMAKE_SOURCE_DIR}/extra/gtest/googletest-${GTEST_VERSION}/build")
SET(GTEST_INSTALL_DIR "${CMAKE_SOURCE_DIR}/extra/gtest/googletest-${GTEST_VERSION}/install")

# Decompress source files
EXECUTE_PROCESS(
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_SOURCE_DIR}/extra/gtest/v${GTEST_VERSION}.tar.gz
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extra/gtest/
    RESULT_VARIABLE tar_result
)
# create build dir
EXECUTE_PROCESS(
    COMMAND mkdir -p ${GTEST_BUILD_DIR}
    WORKING_DIRECTORY ${GTEST_DIR}
    RESULT_VARIABLE mkdir_result
)

SET(GTEST_CMAKE
cmake ${GTEST_SOURCE_DIR}
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_INSTALL_PREFIX=${GTEST_INSTALL_DIR}
    -DBUILD_TESTING=OFF
)

ExternalProject_Add(gtest_external
    SOURCE_DIR ${GTEST_SOURCE_DIR}
    BINARY_DIR ${GTEST_BUILD_DIR}
    STAMP_DIR  ${GTEST_BUILD_DIR}
    CONFIGURE_COMMAND ${GTEST_CMAKE}
    BUILD_COMMAND  make -j${DEFAULT_BUILD_JOBS}
    INSTALL_COMMAND make install -j${DEFAULT_BUILD_JOBS}
)

ADD_LIBRARY(gtest_lib OBJECT IMPORTED)
SET_TARGET_PROPERTIES(
    gtest_lib
    PROPERTIES
    IMPORTED_LOCATION "${GTEST_INSTALL_DIR}/lib/libgtest.a"
)
ADD_DEPENDENCIES(gtest_lib gtest_external)

SET(GTEST_INCLUDE_DIR "${GTEST_INSTALL_DIR}/include" CACHE STRING "Path to gtest include directory")
SET(GTEST_LIBRARIES "${GTEST_INSTALL_DIR}/lib/libgtest.a" CACHE STRING "Path to gtest library")
SET(GTEST_MAIN_LIBRARIES "${GTEST_INSTALL_DIR}/lib/libgtest_main.a" CACHE STRING "gtest main")
