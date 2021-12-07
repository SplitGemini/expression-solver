
project(exp_solver_tests VERSION 0.0.1 LANGUAGES CXX)

add_executable(exp_solver main.cpp)
target_link_libraries(exp_solver 
    PRIVATE 
        libexp_solver
)

add_executable(exp_solver_simple simple_test.cpp)
target_link_libraries(exp_solver_simple 
    PRIVATE 
        libexp_solver
)

add_executable(exp_solver_test unit_tests.cpp)
target_link_libraries(exp_solver_test 
    PRIVATE 
        libexp_solver
)
set_target_properties(exp_solver_test
    PROPERTIES
        CXX_STANDARD 17
)
