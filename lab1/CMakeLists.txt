cmake_minimum_required(VERSION 3.20)

project(Lab1 LANGUAGES CXX)

add_executable(app
        src/main.cpp
        src/grid.cpp
        test.c
)

target_compile_features(app PRIVATE cxx_std_17)

target_include_directories(app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
