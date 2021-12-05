cmake_minimum_required(VERSION 3.1)

project(exp_solver VERSION 0.0.1 LANGUAGES CXX)

set(EXP_SOLVER_MAIN_PROJECT OFF)
if (CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(EXP_SOLVER_MAIN_PROJECT ON)
endif()

add_subdirectory(src)

if(EXP_SOLVER_MAIN_PROJECT)
    add_subdirectory(tests)
endif()
