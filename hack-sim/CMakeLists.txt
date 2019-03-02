cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

set(WARNING_FLAGS -Wall -Wextra -Wconversion
    -Wunreachable-code -Wuninitialized -pedantic-errors -Wold-style-cast
    -Wshadow -Wfloat-equal -Weffc++ -Wmost -Wno-dollar-in-identifier-extension)

project(HackSimulator)

add_executable(HackSimulator src/core/main.cpp)
target_compile_options(HackSimulator PRIVATE ${WARNING_FLAGS})
set_target_properties(HackSimulator PROPERTIES LINK_FLAGS "\
        -s DEMANGLE_SUPPORT=1\
        -s ALLOW_MEMORY_GROWTH=1\
        -s EXPORTED_FUNCTIONS='[\"_ReadProgram\", \"_SetMemoryPtr\",\
        \"_InitializeExecution\", \"_Execute\", \"_SetKey\", \"_Reset\",\
        \"_Clear\"]'\
        -s EXTRA_EXPORTED_RUNTIME_METHODS='[\"ccall\"]'\
        -s DISABLE_EXCEPTION_CATCHING=0\
        --pre-js prefix.js\
        --post-js postfix.js")

add_custom_command(TARGET HackSimulator POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    $<TARGET_FILE_DIR:HackSimulator>/HackSimulator.*
    ${CMAKE_CURRENT_SOURCE_DIR}/src)

