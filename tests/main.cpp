/*

main.cpp

Author: Jingyun Yang
Date Created: 10/2/16

Description: Expression solver that uses
exp_solver.h to solve algebraic expressions.

*/

#include <iostream>
#include "exp_solver.h"

using namespace std;

int main() {
    cout << "| Welcome to expression solver developed by Jingyun Yang!" << endl;
    cout << "| To use this program, type in expressions or declarations for it to solve." << endl;
    cout << "| To quit, enter \"quit\" and press [Enter]." << endl;
    cout << "| Enjoy!" << endl << endl;

    exp_solver::ExpSolver mySolver = exp_solver::ExpSolver();

    string input;

    while (1) {
        cout << "| >> ";

        getline(cin, input);
        if (input == "quit" || input == "q") break;

        auto value = mySolver.SolveExp(input);
        if (value.IsCalculable()) {
            cout << "| " << value.GetValueStr() << endl;
        } else {
            cout << "err: " << mySolver.GetErrorMessages() << endl << endl;
        }
    }
    return 0;
}
