cmake_minimum_required(VERSION 3.25)

project(CMakePackageSuperbuild LANGUAGES NONE)

include(ExternalProject)

set(
    SUPERBUILD_OUTPUT_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/out"
    CACHE PATH
    "Root directory for all superbuild outputs"
)

set(UPSTREAM_INSTALL_PREFIX "${SUPERBUILD_OUTPUT_ROOT}/install/upstream")
set(FMT_INSTALL_PREFIX "${SUPERBUILD_OUTPUT_ROOT}/install/fmt-shared")
set(MYLIB_INSTALL_PREFIX "${SUPERBUILD_OUTPUT_ROOT}/install/mylib")

# Every item below is configured as an independent CMake project. Dependencies
# determine the build order; package *_DIR values always point at install trees.
ExternalProject_Add(
    upstream
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/upstream"
    BINARY_DIR "${SUPERBUILD_OUTPUT_ROOT}/build/upstream/${CMAKE_BUILD_TYPE}"
    INSTALL_DIR "${UPSTREAM_INSTALL_PREFIX}"
    CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DBUILD_TESTING=ON"
    TEST_COMMAND
        "${CMAKE_CTEST_COMMAND}" --test-dir <BINARY_DIR> --output-on-failure
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
    BINARY_DIR "${SUPERBUILD_OUTPUT_ROOT}/build/fmt/${CMAKE_BUILD_TYPE}"
    INSTALL_DIR "${FMT_INSTALL_PREFIX}"
    CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DBUILD_SHARED_LIBS=ON"
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
    BINARY_DIR "${SUPERBUILD_OUTPUT_ROOT}/build/mylib/${CMAKE_BUILD_TYPE}"
    INSTALL_DIR "${MYLIB_INSTALL_PREFIX}"
    CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DUpstream_DIR=${UPSTREAM_INSTALL_PREFIX}/lib/cmake/Upstream"
        "-Dfmt_DIR=${FMT_INSTALL_PREFIX}/lib/cmake/fmt"
        "-DBUILD_TESTING=ON"
    TEST_COMMAND
        "${CMAKE_COMMAND}" -E env
        --modify "PATH=path_list_prepend:${FMT_INSTALL_PREFIX}/bin"
        "${CMAKE_CTEST_COMMAND}" --test-dir <BINARY_DIR> --output-on-failure
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
    BINARY_DIR "${SUPERBUILD_OUTPUT_ROOT}/build/consumer/${CMAKE_BUILD_TYPE}"
    CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
        "-DMyLib_DIR=${MYLIB_INSTALL_PREFIX}/lib/cmake/MyLib"
        "-DUpstream_DIR=${UPSTREAM_INSTALL_PREFIX}/lib/cmake/Upstream"
        "-Dfmt_DIR=${FMT_INSTALL_PREFIX}/lib/cmake/fmt"
        "-DBUILD_TESTING=ON"
    TEST_COMMAND
        "${CMAKE_COMMAND}" -E env
        --modify "PATH=path_list_prepend:${FMT_INSTALL_PREFIX}/bin"
        "${CMAKE_CTEST_COMMAND}" --test-dir <BINARY_DIR> --output-on-failure
    INSTALL_COMMAND ""
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_TEST TRUE
)
