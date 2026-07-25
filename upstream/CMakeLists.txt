cmake_minimum_required(VERSION 3.25)

project(
    Upstream
    VERSION 1.0.0
    DESCRIPTION "An external dependency used by MyLib"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

add_library(upstream src/message.cpp)
add_library(Upstream::upstream ALIAS upstream)

target_compile_features(upstream PUBLIC cxx_std_11)
target_include_directories(
    upstream
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
    upstream
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        EXPORT_NAME upstream
)

if(MSVC)
    target_compile_options(upstream PRIVATE /W4 /permissive-)
else()
    target_compile_options(upstream PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(upstream_test tests/message_test.cpp)
    target_link_libraries(upstream_test PRIVATE Upstream::upstream)
    add_test(NAME upstream.message COMMAND upstream_test)
endif()

set(UPSTREAM_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/Upstream")

install(
    TARGETS upstream
    EXPORT UpstreamTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(DIRECTORY include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(
    EXPORT UpstreamTargets
    FILE UpstreamTargets.cmake
    NAMESPACE Upstream::
    DESTINATION ${UPSTREAM_INSTALL_CMAKEDIR}
)

configure_package_config_file(
    cmake/UpstreamConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/UpstreamConfig.cmake"
    INSTALL_DESTINATION ${UPSTREAM_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/UpstreamConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/UpstreamConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/UpstreamConfigVersion.cmake"
    DESTINATION ${UPSTREAM_INSTALL_CMAKEDIR}
)
