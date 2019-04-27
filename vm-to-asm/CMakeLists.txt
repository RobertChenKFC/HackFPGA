cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

project(VMTranslator)

# TODO: change all CMakeLists.txt to use gcc
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(WARNING_FLAGS -Wall -Wextra -Wconversion
      -Wunreachable-code -Wuninitialized -pedantic-errors -Wold-style-cast
      -Wshadow -Wfloat-equal -Weffc++ -Wno-dollar-in-identifier-extension)
endif()

add_executable(VMTranslator src/main.cpp)
target_compile_options(VMTranslator PRIVATE ${WARNING_FLAGS})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    target_link_libraries(VMTranslator PRIVATE stdc++fs)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    target_link_libraries(VMTranslator PRIVATE c++fs)
endif()
