cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(WARNING_FLAGS -Wall -Wextra -Wconversion
    -Wunreachable-code -Wuninitialized -pedantic-errors -Wold-style-cast
    -Wshadow -Wfloat-equal -Weffc++ -Wmost -Wno-dollar-in-identifier-extension)

project(HackAssembler)

add_executable(HackAssembler src/main.cpp)
target_compile_options(HackAssembler PRIVATE ${WARNING_FLAGS})

