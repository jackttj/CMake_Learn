cmake_minimum_required(VERSION 3.25)

project(CMakePackageSuperbuild LANGUAGES NONE)

include(ExternalProject)

set(
    SUPERBUILD_OUTPUT_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/out"
    CACHE PATH
    "Root directory for all superbuild outputs"
)

set(
    SUPERBUILD_VARIANT
    "default"
    CACHE STRING
    "Output variant, including the selected toolchain"
)

set(
    SUPERBUILD_BUILD_CONFIG
    "Release"
    CACHE STRING
    "Configuration built by a multi-config generator"
)

set(
    UPSTREAM_INSTALL_PREFIX
    "${SUPERBUILD_OUTPUT_ROOT}/install/upstream/${SUPERBUILD_VARIANT}"
)
set(
    FMT_INSTALL_PREFIX
    "${SUPERBUILD_OUTPUT_ROOT}/install/third-part/fmt/${SUPERBUILD_VARIANT}-shared"
)
set(
    MYLIB_INSTALL_PREFIX
    "${SUPERBUILD_OUTPUT_ROOT}/install/mylib/${SUPERBUILD_VARIANT}"
)

set(SUPERBUILD_COMMON_CMAKE_ARGS)
if(NOT CMAKE_CONFIGURATION_TYPES)
    list(
        APPEND SUPERBUILD_COMMON_CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=${SUPERBUILD_BUILD_CONFIG}"
    )
endif()

# Every item below is configured as an independent CMake project. Dependencies
# determine the build order; package *_DIR values always point at install trees.
ExternalProject_Add(
    upstream
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/upstream"
    BINARY_DIR "${SUPERBUILD_OUTPUT_ROOT}/build/upstream/${SUPERBUILD_VARIANT}"
    INSTALL_DIR "${UPSTREAM_INSTALL_PREFIX}"
    CMAKE_ARGS
        ${SUPERBUILD_COMMON_CMAKE_ARGS}
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DBUILD_TESTING=ON"
    TEST_COMMAND
        "${CMAKE_CTEST_COMMAND}"
        --test-dir <BINARY_DIR>
        --build-config "${SUPERBUILD_BUILD_CONFIG}"
        --output-on-failure
    TEST_BEFORE_INSTALL TRUE
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_TEST TRUE
    USES_TERMINAL_INSTALL TRUE
)

# fmt is another independent dependency of MyLib. It can build alongside
# Upstream, but both must be installed before MyLib is configured.
ExternalProject_Add(
    fmt
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third-part/fmt"
    BINARY_DIR "${SUPERBUILD_OUTPUT_ROOT}/build/third-part/fmt/${SUPERBUILD_VARIANT}-shared"
    INSTALL_DIR "${FMT_INSTALL_PREFIX}"
    CMAKE_ARGS
        ${SUPERBUILD_COMMON_CMAKE_ARGS}
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DBUILD_SHARED_LIBS=OFF"
        "-DFMT_DOC=OFF"
        "-DFMT_TEST=OFF"
        "-DFMT_INSTALL=ON"
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_INSTALL TRUE
)

ExternalProject_Add(
    mylib
    DEPENDS upstream fmt
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/mylib"
    BINARY_DIR "${SUPERBUILD_OUTPUT_ROOT}/build/mylib/${SUPERBUILD_VARIANT}"
    INSTALL_DIR "${MYLIB_INSTALL_PREFIX}"
    CMAKE_ARGS
        ${SUPERBUILD_COMMON_CMAKE_ARGS}
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DUpstream_DIR=${UPSTREAM_INSTALL_PREFIX}/lib/cmake/Upstream"
        "-Dfmt_DIR=${FMT_INSTALL_PREFIX}/lib/cmake/fmt"
        "-DBUILD_TESTING=ON"
    TEST_COMMAND
        "${CMAKE_COMMAND}" -E env
        --modify "PATH=path_list_prepend:${FMT_INSTALL_PREFIX}/bin"
        "${CMAKE_CTEST_COMMAND}"
        --test-dir <BINARY_DIR>
        --build-config "${SUPERBUILD_BUILD_CONFIG}"
        --output-on-failure
    TEST_BEFORE_INSTALL TRUE
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_TEST TRUE
    USES_TERMINAL_INSTALL TRUE
)

ExternalProject_Add(
    consumer
    DEPENDS mylib
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/consumer"
    BINARY_DIR "${SUPERBUILD_OUTPUT_ROOT}/build/consumer/${SUPERBUILD_VARIANT}"
    CMAKE_ARGS
        ${SUPERBUILD_COMMON_CMAKE_ARGS}
        "-DMyLib_DIR=${MYLIB_INSTALL_PREFIX}/lib/cmake/MyLib"
        "-DUpstream_DIR=${UPSTREAM_INSTALL_PREFIX}/lib/cmake/Upstream"
        "-Dfmt_DIR=${FMT_INSTALL_PREFIX}/lib/cmake/fmt"
        "-DBUILD_TESTING=ON"
    TEST_COMMAND
        "${CMAKE_COMMAND}" -E env
        --modify "PATH=path_list_prepend:${FMT_INSTALL_PREFIX}/bin"
        "${CMAKE_CTEST_COMMAND}"
        --test-dir <BINARY_DIR>
        --build-config "${SUPERBUILD_BUILD_CONFIG}"
        --output-on-failure
    INSTALL_COMMAND ""
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_TEST TRUE
)
