cmake_minimum_required(VERSION 3.25)

project(MyLibConsumer VERSION 1.0.0 LANGUAGES CXX)

find_package(MyLib 1.0 CONFIG REQUIRED)

add_subdirectory(MathLib)


add_executable(consumer src/main.cpp)
target_compile_features(consumer PRIVATE cxx_std_11)
target_link_libraries(consumer 
                        PRIVATE 
                        MathLib
                        MyLib::mylib)

include(CTest)
if(BUILD_TESTING)
    add_test(NAME consumer.runs COMMAND consumer)
    set_tests_properties(
        consumer.runs
        PROPERTIES PASS_REGULAR_EXPRESSION "Hello, downstream consumer!"
    )
endif()
