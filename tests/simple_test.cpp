#include <iostream>
#include <string>
#include <regex>
#include <iomanip>
#include <chrono>
#include <regex>
#include <cmath>

#include "exp_solver.h"
#include "value.h"


int main() {
    exp_solver::ExpSolver s;

    auto result = s.SolveExp("10**19");
    std::cout << "result: " << result << ", error: " << s.GetErrorMessages() << std::endl;

    result = s.SolveExp("--1/1");
    std::cout << "result: " << result << ", error: " << s.GetErrorMessages() << std::endl;

    result = s.SolveExp("1**-0.1");
    std::cout << "result: " << result << ", error: " << s.GetErrorMessages() << std::endl;

    result = s.SolveExp("-1-1");
    std::cout << "result: " << result << ", error: " << s.GetErrorMessages() << std::endl;

    result = s.SolveExp("exp()");
    std::cout << "result: " << result << ", error: " << s.GetErrorMessages() << std::endl;

    result = s.SolveExp("sin(pi*2)");
    std::cout << "result: " << result << ", error: " << s.GetErrorMessages() << std::endl;

    using namespace std::chrono;
    auto now = steady_clock::now();
    // for (size_t i = 0; i < 10000; i++) { s.SolveExp("1+((2-3*4)/5)**6%4"); }
    std::cout << "10000 time calculate time used: "
              << duration_cast<milliseconds>(steady_clock::now() - now).count() << "ms"
              << std::endl;

    return 0;
}