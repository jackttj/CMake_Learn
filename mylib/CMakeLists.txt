cmake_minimum_required(VERSION 3.25)

project(
    MyLib
    VERSION 1.0.0
    DESCRIPTION "A small installable CMake library"
    LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

find_package(Upstream 1.0 CONFIG REQUIRED)
find_package(fmt  REQUIRED)
set(MYLIB_UPSTREAM_DIR "${Upstream_DIR}")
set(MYLIB_FMT_DIR "${fmt_DIR}")


add_library(mylib src/greeting.cpp)
add_library(MyLib::mylib ALIAS mylib)

target_compile_features(mylib PUBLIC cxx_std_11)
target_link_libraries(mylib PUBLIC Upstream::upstream)
target_link_libraries(mylib PUBLIC fmt::fmt-c)
target_include_directories(
    mylib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(
    mylib
    PROPERTIES
        VERSION ${PROJECT_VERSION}
        SOVERSION ${PROJECT_VERSION_MAJOR}
        EXPORT_NAME mylib
)

if(MSVC)
    target_compile_options(mylib PRIVATE /W4 /permissive-)
else()
    target_compile_options(mylib PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(CTest)
if(BUILD_TESTING)
    add_executable(mylib_test tests/greeting_test.cpp)
    target_link_libraries(mylib_test PRIVATE MyLib::mylib)
    add_test(NAME mylib.greeting COMMAND mylib_test)
endif()

set(MYLIB_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/MyLib")

install(
    TARGETS mylib
    EXPORT MyLibTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT MyLibTargets
    FILE MyLibTargets.cmake
    NAMESPACE MyLib::
    DESTINATION ${MYLIB_INSTALL_CMAKEDIR}
)

configure_package_config_file(
    cmake/MyLibConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/MyLibConfig.cmake"
    INSTALL_DESTINATION ${MYLIB_INSTALL_CMAKEDIR}
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/MyLibConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        "${CMAKE_CURRENT_BINARY_DIR}/MyLibConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/MyLibConfigVersion.cmake"
    DESTINATION ${MYLIB_INSTALL_CMAKEDIR}
)
