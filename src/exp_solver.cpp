/*

exp_solver.cpp

Author: Jingyun Yang, SplitGemini
Date Created: 1/16/15
Modified: 4/12/2021

Description: Implementation file for ExpSolver
class that takes in algebraic expressions as
strings and solves them.

*/

#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <regex>
#include <cctype>
#include <cmath>

#include "exp_solver.h"


namespace exp_solver
{
using std::string;
using std::vector;

// ******************** //
// * Public Functions * //
// ******************** //

// Constructor
ExpSolver::ExpSolver() {
    AddPredefined();
}


Value ExpSolver::ResolveExp() {
    if (expression.empty()) {
        error_messages << "not solve a expression first\n";
        return {};
    }
    error_messages.clear();
    error_messages.str("");
    // Recursively solve the expression
    Value result = CalculateExp(expression, 0, blocks.size());

    if (result.IsCalculable()) {
        // Create output string
        return result;
    } else {
        // blocks.clear();
        auto value_error = result.GetErrorMessage();
        error_messages << "Calculation aborted"
                       << (value_error.empty() ? "" : (", cuz: " + value_error)) << std::endl;
        return {};
    }

    return {};
}

Value ExpSolver::SolveExp(const string &input) {
    // clean old result
    blocks.clear();
    error_messages.clear();
    error_messages.str("");

    // Discard all spaces in the expression
    auto exp = input;
    exp.erase(std::remove_if(exp.begin(), exp.end(), ::isspace), exp.end());

    // Deal with no right-hand-side input
    if (exp.length() == 0) {
        error_messages << "Invalid expression! " << std::endl;
        blocks.clear();
        return {};
    }

    // Deal with negative signs in the expression
    DealWithNegativeSign(exp);

    // Group the expression into substrings
    // and calculate the bracket level of each substring
    bool groupSucceed = GroupExp(exp);

    if (!groupSucceed) {
        blocks.clear();
        error_messages << "Calculation aborted. " << std::endl;
        return {};
    }

    // Recursively solve the expression
    Value result = CalculateExp(exp, 0, blocks.size());

    if (result.IsCalculable()) {
        expression = exp;
        // Create output
        return result;
    } else {
        blocks.clear();
        auto value_error = result.GetErrorMessage();
        error_messages << "Calculation aborted"
                       << (value_error.empty() ? "" : (", cuz: " + value_error)) << std::endl;
        return {};
    }

    return {};
}


std::string ExpSolver::GetErrorMessages() const {
    return error_messages.str();
}


bool ExpSolver::AddOrEditConstant(const std::string &name, const Value &value) {
    if (!value.IsCalculable()) {
        error_messages << value.GetErrorMessage();
        return false;
    }
    for (size_t i = 0; i < constants.size(); i++) {
        if (name.compare(constants[i].name) == 0) {
            constants[i].value = value;
            return true;
        }
    }
    constants.push_back(Variable(name, value));
    return true;
}


// ********************* //
// * Private Functions * //
// ********************* //

// Add predefined constants and functions
void ExpSolver::AddPredefined() {
    constants.push_back(Variable("e", Value(M_E)));
    constants.push_back(Variable("pi", Value(M_PI)));
    // constants.push_back(Variable("ans", Value()));
    functions.push_back(Function("sin", std::sin));
    functions.push_back(Function("cos", std::cos));
    functions.push_back(Function("tan", std::tan));
    functions.push_back(Function("exp", std::exp));
    functions.push_back(Function("sqrt", std::sqrt));
    functions.push_back(Function("floor", std::floor));
    functions.push_back(Function("ceil", std::ceil));
    functions.push_back(Function("round", round));
    functions.push_back(Function("ln", std::log));
    functions.push_back(Function("log", std::log10));
    functions.push_back(Function("abs", std::abs));
}


// reference: https://docs.python.org/zh-cn/3.7/reference/expressions.html#operator-precedence
// The smaller the ordinal, the higher the priority
static const vector<vector<string>> sym_priority_vec = {
    { "**" }, { "~" }, { "*", "/", "//", "%" }, { "+", "-" }, { "<<", ">>" }, { "&" },
    { "^" },  { "|" }
};

static void SetPriority(const string &exp, Block &block) {
    if (block.type != BlockType::Sym) return;
    auto sym = exp.substr(block.start, block.end - block.start);
    for (size_t i = 0; i < sym_priority_vec.size(); i++) {
        bool found = false;
        for (size_t k = 0; k < sym_priority_vec[i].size(); k++) {
            if (sym == sym_priority_vec[i][k]) {
                block.priority = i;
                found          = true;
                break;
            }
        }
        if (found) break;
    }
}

// This is similar to lexical analysis in a compiler
// Partition an expression into blocks of different types
bool ExpSolver::GroupExp(const string &exp) {
    // Record the start of next push and current level
    int start = 0, level = 0;

    // Record type of the last character
    BlockType lastType = Nil;


    for (size_t i = 0; i <= exp.length(); i++) {
        // Record type of the just inspected character
        BlockType thisType = Char2Type(exp[i]);

        // Check if new block needs to be pushed into stack
        bool needNewBlock = false;
        needNewBlock |= (lastType == BracL);
        needNewBlock |= (lastType == BracR);
        // symbol may has 2 char
        // needNewBlock |= (lastType == Sym);
        needNewBlock |= (thisType != lastType);
        needNewBlock |= (i == exp.length());
        // once encounter ~, new a block
        needNewBlock |= exp[i] == '~';
        // function can has num in name
        needNewBlock &= !(thisType == Num && lastType == Func);

        if (needNewBlock) {
            // Label string as function, constant or variable
            if (lastType == Func) {
                lastType = AnalyzeStrType(exp.substr(start, i - start));
                if (lastType == Nil) return false;
            }

            // Push new block into stack
            // if it's not the case where there is '(' at start
            if (i != 0) {
                auto newBlock = Block(start, i, level, lastType);
                SetPriority(exp, newBlock);
                blocks.push_back(newBlock);
#if EXP_SOLVER_DEBUG
                std::cout << exp.substr(newBlock.start, newBlock.end - newBlock.start) << std::endl;
#endif
            }

            // Lower level after right brackets are pushed into stack
            if (lastType == BracR) level--;

            // Update lastType
            lastType = thisType;
            start    = i;
        }

        // Upper level before left brackets are pushed into stack
        if (thisType == BracL) level++;
    }

    // Throw error if brackets are not paired
    // (diagonized by inspecting variable level)
    if (level != 0) {
        error_messages << "Syntax error: Brackets not paired! " << std::endl;
        return false;
    }

    return true;
}

// Analyze whether a string Block is of BlockType Func, Constant or Var
BlockType ExpSolver::AnalyzeStrType(const string &str) {
    for (size_t i = 0; i < functions.size(); i++) {
        if (str.compare(functions[i].name) == 0) return Func;
    }
    for (size_t i = 0; i < constants.size(); i++) {
        if (str.compare(constants[i].name) == 0) return Constant;
    }
    for (size_t i = 0; i < variables.size(); i++) {
        if (str.compare(variables[i].name) == 0) return Var;
    }
    error_messages << "String \"" << str << "\" not recognized! " << std::endl;
    return Nil;
}

static const string sym_char{ "+-*/^%&|<>~" };

// Determine the type of one single character
BlockType ExpSolver::Char2Type(char c) {
    if (c == '_' || isalpha(c))
        return Func;
    else if (c == '.' || isdigit(c))
        return Num;
    else if (c == '(')
        return BracL;
    else if (c == ')')
        return BracR;
    else if (sym_char.find(c) != sym_char.npos)
        return Sym;
    else
        return Nil;
}

// prefix with sym_char replace to (0-)
// example expression: "-2*3" and "3*-2", will replace to "(0-2)*3" and "3*(0-2)"
static const std::regex negative_pattern{ "([+\\-*\\/^%&|<>~]|^)(-[\\d.]+)" };
// "-(1+1)" -> "0-(1+1)", "(-2)+1" -> "(0-2)+1"
static const std::regex negative_pattern2{ "(\\(|^)(?=-(?:[\\d.]+|\\())" };

// Replace every "-" as negative sign by "0-"
void ExpSolver::DealWithNegativeSign(string &exp) {
    exp = std::regex_replace(exp, negative_pattern, "$1(0$2)");
#if EXP_SOLVER_DEBUG
    std::cout << "replace to:" << exp << std::endl;
#endif
    exp = std::regex_replace(exp, negative_pattern2, "$010");
#if EXP_SOLVER_DEBUG
    std::cout << "replace to:" << exp << std::endl;
#endif
}

// Calculate expression in block range [startBlock,endBlock)
Value ExpSolver::CalculateExp(const string &exp, int startBlock, int endBlock) {
    // Create stacks that stores operands and operators
    std::stack<Value> values;
    std::stack<Block> ops;

    for (int i = endBlock - 1; i >= startBlock; i--) {
        // Record the content of the current block
        string blockStr = exp.substr(blocks[i].start, blocks[i].end - blocks[i].start);

        // Variable to prepare for skipping items inside the for loop
        // When this method is recursively called to calculate in-bracket contents
        // this variable is modified
        int iIncrement = 0;

        // Push numbers to stack
        if (blocks[i].type == Num) {
            values.push(Value(blockStr));
        }

        // Throw error if function names occur without brackets
        else if (blocks[i].type == Func) {
            error_messages << "Syntax Error: Need brackets after function name! " << std::endl;
            return {};
        }

        // Replace constants with Value
        else if (blocks[i].type == Constant) {
            Value constValue;
            for (size_t i = 0; i < constants.size(); i++) {
                if (blockStr.compare(constants[i].name) == 0) {
                    constValue = constants[i].value;

                    // If constant doesn't have a value
                    // that means that 'ans' is not defined
                    if (!constValue.IsCalculable()) {
                        error_messages
                            << "Bad access: \"" << constants[i].name
                            << "\" not defined currently! cuz: " << constValue.GetErrorMessage()
                            << std::endl;
                        return {};
                    }

                    break;
                }
            }
            values.push(constValue);
        }

        // Replace values with Value
        else if (blocks[i].type == Var) {
            Value varValue;
            for (size_t i = 0; i < variables.size(); i++) {
                if (blockStr.compare(variables[i].name) == 0) {
                    varValue = variables[i].value;
                    break;
                }
            }
            values.push(varValue);
        }

        // Recursively solve expression inside brackets
        // and modify iIncrement to skip in-bracket loop items
        else if (blocks[i].type == BracR) {
            // Corresponding Block ID
            int corBlock = FindIndexOfBracketEnding(i);
            if (corBlock != 0 && blocks[corBlock - 1].type == Func) {
                // Find the function and calculate the result of the function.
                double (*funcToUse)(double);
                string funcName = exp.substr(blocks[corBlock - 1].start,
                                             blocks[corBlock - 1].end - blocks[corBlock - 1].start);
                for (size_t i = 0; i < functions.size(); i++) {
                    if (funcName.compare(functions[i].name) == 0) {
                        funcToUse = functions[i].func;
                        break;
                    }
                }
                Value valueInFunc = CalculateExp(exp, corBlock + 1, i);
                if (!valueInFunc.IsCalculable()) {
                    return {};
                } else if (valueInFunc.GetValueDouble() < 0 && funcName.compare("sqrt") == 0) {
                    error_messages << "Arithmetic error: Cannot square root a negative number! "
                                   << std::endl;
                    return {};
                }
                double funcResult = (*funcToUse)(valueInFunc.GetValueDouble());

                // Convert to Value and push to stack.
                values.push(Value(funcResult));
                iIncrement -= i - corBlock + 1;
            } else {
                // only recursive brace
                Value valueToPush = CalculateExp(exp, corBlock + 1, i);
                if (!valueToPush.IsCalculable()) { return {}; }
                values.push(valueToPush);
                iIncrement -= i - corBlock;
            }
        }

        // Push symbols into stack and calculate
        // from highest calculation priority down
        else if (blocks[i].type == Sym) {
            auto priority = blocks[i].priority;
            while (!ops.empty()) {
                // process all priority num less(which mean priority higher) op
                if (ops.top().priority < priority) {
                    auto op = ops.top();
                    ops.pop();
                    auto currentBlock = exp.substr(op.start, op.end - op.start);
                    // need pop 2 value
                    if (currentBlock != "~") {
                        Value v1 = values.top();
                        values.pop();
                        Value v2 = values.top();
#if EXP_SOLVER_DEBUG
                        std::cout << v1 << currentBlock << v2 << std::endl;
#endif // EXP_SOLVER_DEBUG
                        values.pop();
                        values.push(v1.operate(currentBlock, v2));
                    } else {
                        // Negate op, pop one value
                        Value v1 = values.top();
                        values.pop();
                        values.push(~v1);
                    }
                } else
                    break;
            }

            ops.push(blocks[i]);
        } else {
            error_messages << "Encountered unknown character at " << blockStr << "!" << std::endl;
            return {};
        }

        i += iIncrement;
    }

    // Final calculation not containing any bracket
    // if (values.size() != ops.size() + 1) {
    //     error_messages << "Invalid expression! " << std::endl;
    //     return {};
    // }
#if EXP_SOLVER_DEBUG
    std::cout << "remained" << std::endl;
#endif // EXP_SOLVER_DEBUG
    while (!ops.empty()) {
        // values empty, no need to cal
        if (values.empty()) break;

        auto op = ops.top();
        ops.pop();
        // Record the content of the current block
        string currentBlock = exp.substr(op.start, op.end - op.start);

        if (currentBlock != "~") {
            if (values.empty()) {
                error_messages << "Invalid expression! ";
                return {};
            }
            Value v1 = values.top();
            values.pop();
            if (values.empty()) {
                error_messages << "Invalid expression! ";
                return {};
            }
            Value v2 = values.top();
            values.pop();

#if EXP_SOLVER_DEBUG
            std::cout << v1 << currentBlock << v2 << std::endl;
#endif // EXP_SOLVER_DEBUG

            values.push(v1.operate(currentBlock, v2));
        } else {
            if (values.empty()) {
                error_messages << "Invalid expression! ";
                return {};
            }
            Value v1 = values.top();
            values.pop();
            values.push(~v1);
        }
    }
    // remain other value or ops, invalid
    if (values.size() != 1 || !ops.empty()) {
        error_messages << "Invalid expression! " << std::endl;
        return {};
    }
    return values.top();
}

// Given the block id of ')', find the block id of corresponding '('
int ExpSolver::FindIndexOfBracketEnding(int blockId) {
    int levelToFind = blocks[blockId].level - 1, currentBlockId = blockId;
    while (blocks[currentBlockId].level != levelToFind) {
        if (currentBlockId == 0) return currentBlockId;
        currentBlockId--;
    }
    return currentBlockId + 1;
}
} // namespace exp_solver