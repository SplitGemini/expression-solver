project(exp_solver_lib VERSION 0.0.1 LANGUAGES CXX)

add_library(libexp_solver 
    STATIC 
        exp_solver.cpp
        value.cpp
)

target_include_directories(libexp_solver
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>
        $<INSTALL_INTERFACE:include>
)

set_target_properties(libexp_solver 
    PROPERTIES
        OUTPUT_NAME exp_solver
        CXX_STANDARD 17
)
