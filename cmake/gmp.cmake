INCLUDE(ExternalProject)

MESSAGE(STATUS "============================================")
MESSAGE(STATUS "=           Building gmp                   =")
MESSAGE(STATUS "============================================")

SET(GMP_VERSION 6.3.0)
SET(GMP_DIR         "${CMAKE_SOURCE_DIR}/extra/gmp/")
SET(GMP_TAR_BALL    "${CMAKE_SOURCE_DIR}/extra/gmp/gmp-${GMP_VERSION}.tar.xz")
SET(GMP_SOURCE_DIR  "${CMAKE_SOURCE_DIR}/extra/gmp/gmp-${GMP_VERSION}/")
SET(GMP_BUILD_DIR   "${CMAKE_SOURCE_DIR}/extra/gmp/gmp-${GMP_VERSION}/gmp_build")
SET(GMP_INSTALL_DIR "${CMAKE_SOURCE_DIR}/extra/gmp/gmp-${GMP_VERSION}/gmp_install")

# Decompress source files
EXECUTE_PROCESS(
    COMMAND ${CMAKE_COMMAND} -E tar xf ${CMAKE_SOURCE_DIR}/extra/gmp/gmp-${GMP_VERSION}.tar.xz
    WORKING_DIRECTORY ${GMP_DIR}
    RESULT_VARIABLE tar_result
)
# create build dir
EXECUTE_PROCESS(
    COMMAND mkdir -p ${GMP_BUILD_DIR}
    WORKING_DIRECTORY ${GMP_DIR}
    RESULT_VARIABLE mkdir_result
)

ExternalProject_Add(gmp_external
    SOURCE_DIR ${GMP_SOURCE_DIR}
    BINARY_DIR ${GMP_BUILD_DIR}
    STAMP_DIR  ${GMP_BUILD_DIR}
    CONFIGURE_COMMAND ${GMP_SOURCE_DIR}/configure
        --prefix=${GMP_INSTALL_DIR} CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
    BUILD_COMMAND  make -j
    INSTALL_COMMAND make install -j
)

ADD_LIBRARY(gmp_static_lib OBJECT IMPORTED)
ADD_DEPENDENCIES(gmp_static_lib gmp_external)

ADD_LIBRARY(gmp_shared_lib SHARED IMPORTED)
ADD_DEPENDENCIES(gmp_shared_lib gmp_external)

SET(GMP_INCLUDE_DIR "${GMP_INSTALL_DIR}/include" CACHE STRING "gmp include directory")
SET(GMP_LIB_DIR "${GMP_INSTALL_DIR}/lib" CACHE STRING "gmp lib directory")
SET(GMP_LIBRARIES "${GMP_INSTALL_DIR}/lib/libgmp.a" CACHE STRING "gmp library")
SET(GMP_SHARED_LIBRARIES "${GMP_INSTALL_DIR}/lib/libgmp.so" CACHE STRING "gmp shared library")
