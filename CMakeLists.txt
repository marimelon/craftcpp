cmake_minimum_required(VERSION 3.4...3.18)
project(craftcpp)

set(CMAKE_CXX_STANDARD 20)

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/std:c++latest>")

add_subdirectory(pybind11)
pybind11_add_module(craftcpp src/py_main.cpp src/CrafterCore.cpp src/Finisher.cpp src/State.cpp)

# EXAMPLE_VERSION_INFO is defined by setup.py and passed into the C++ code as a
# define (VERSION_INFO) here.
target_compile_definitions(craftcpp
                           PRIVATE VERSION_INFO=${EXAMPLE_VERSION_INFO})
