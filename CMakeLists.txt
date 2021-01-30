cmake_minimum_required(VERSION 3.19)
project(determinant)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_DISABLE_PRECOMPILE_HEADERS ON)

if (MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()


add_subdirectory(solver)
add_subdirectory(measure)
add_subdirectory(tests)

set(SOURCES main.cpp)

add_executable(determinant ${SOURCES})
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT determinant)

target_link_libraries(determinant solverlib measurelib)
