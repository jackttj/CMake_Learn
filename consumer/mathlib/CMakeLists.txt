cmake_minimum_required(VERSION 3.25)

project(MathLIB VERSION 1.0.0 LANGUAGES CXX)

add_library(MathLib src/mymath.cpp)
target_compile_features(MathLib PRIVATE cxx_std_11)

target_include_directories(MathLib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

