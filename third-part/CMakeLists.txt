cmake_minimum_required(VERSION 3.25)

project(ThirdPartySuperbuild LANGUAGES NONE)

include(ExternalProject)

option(
    THIRD_PARTY_BUILD_SHARED_LIBS
    "Build third-party libraries as shared libraries"
    OFF
)

set(
    THIRD_PARTY_BUILD_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/../out/build/third-part"
    CACHE PATH
    "Root directory for third-party build trees"
)

set(
    THIRD_PARTY_INSTALL_ROOT
    "${CMAKE_CURRENT_SOURCE_DIR}/../out/install/third-part"
    CACHE PATH
    "Root directory for third-party install trees"
)

set(
    THIRD_PARTY_VARIANT
    "default"
    CACHE STRING
    "Output subdirectory identifying the selected preset"
)

set(
    THIRD_PARTY_COMMON_CMAKE_ARGS
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=${CMAKE_EXPORT_COMPILE_COMMANDS}"
)

# CMAKE_BUILD_TYPE belongs to single-config generators such as MinGW Makefiles.
# Visual Studio selects its configuration at build time via the Build Preset.
if(NOT CMAKE_CONFIGURATION_TYPES)
    list(
        APPEND THIRD_PARTY_COMMON_CMAKE_ARGS
        "-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}"
    )
endif()

ExternalProject_Add(
    fmt
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/fmt"
    BINARY_DIR "${THIRD_PARTY_BUILD_ROOT}/fmt/${THIRD_PARTY_VARIANT}"
    INSTALL_DIR "${THIRD_PARTY_INSTALL_ROOT}/fmt/${THIRD_PARTY_VARIANT}"
    CMAKE_ARGS
        ${THIRD_PARTY_COMMON_CMAKE_ARGS}
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DBUILD_SHARED_LIBS=${THIRD_PARTY_BUILD_SHARED_LIBS}"
        "-DFMT_DOC=OFF"
        "-DFMT_TEST=OFF"
        "-DFMT_FUZZ=OFF"
        "-DFMT_INSTALL=ON"
    UPDATE_COMMAND ""
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_INSTALL TRUE
)

ExternalProject_Add(
    yaml-cpp
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp"
    BINARY_DIR "${THIRD_PARTY_BUILD_ROOT}/yaml-cpp/${THIRD_PARTY_VARIANT}"
    INSTALL_DIR "${THIRD_PARTY_INSTALL_ROOT}/yaml-cpp/${THIRD_PARTY_VARIANT}"
    CMAKE_ARGS
        ${THIRD_PARTY_COMMON_CMAKE_ARGS}
        "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
        "-DBUILD_TESTING=OFF"
        "-DYAML_BUILD_SHARED_LIBS=${THIRD_PARTY_BUILD_SHARED_LIBS}"
        "-DYAML_CPP_BUILD_TESTS=OFF"
        "-DYAML_CPP_BUILD_TOOLS=OFF"
        "-DYAML_CPP_FORMAT_SOURCE=OFF"
        "-DYAML_CPP_INSTALL=ON"
    UPDATE_COMMAND ""
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    USES_TERMINAL_INSTALL TRUE
)
