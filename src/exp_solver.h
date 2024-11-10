/*

exp_solver.h

Author: Jingyun Yang, SplitGemini
Date Created: 1/16/15
Modified: 4/12/2021

Description: Header file for ExpSolver class
that takes in algebraic expressions as std::strings
and solves them.

*/
#pragma once
#include <string>
#include <vector>
#include <stack>
#include <sstream>
#include "value.h"

namespace exp_solver
{
struct Variable {
    std::string name;
    Value       value;
    Variable(std::string nm, Value val) : name(nm), value(val) {}
};

struct Function {
    std::string name;
    double (*func)(double);
    Function(std::string nm, double (*f)(double)) : name(nm), func(f) {}
};

enum BlockType { Num, Sym, Func, Constant, Var, BracL, BracR, Nil };

struct Block {
    int       start, end, level, priority;
    BlockType type;
    Block() : start(0), end(0), level(0), priority(INT32_MAX), type(Nil) {}
    Block(int s, int e, int l, BlockType tp) :
        start(s), end(e), level(l), priority(INT32_MAX), type(tp) {}
};

class ExpSolver {
public:
    // Constructor to initialize the ExpSolver object
    ExpSolver();

    // set expression
    void SetExp(const std::string &exp);

    /**
     * @brief use old expression resolve again, must use solveExp first
     * @note use getErrorMessages() get fail reason
     * @return result of output, empty for fail
     */
    Value ResolveExp();
    /**
     * @brief calculate expression and output result
     *
     * @param exp expression to be calculated
     * @return result of output
     */
    Value SolveExp(const std::string &exp);

    std::string GetErrorMessages() const;

    /**
     * @brief set or edit a constant variable with value
     * @note use getErrorMessages() get fail reason
     * @example
     * ExpSolver exp;
     * exp.UpdateVariable("test", 10);
     * auto output = exp.solveExp("test + 10"); // output will be 20
     * exp.UpdateVariable("test", 20);
     * output = exp.resolveExp(); // output will be 30
     * @param name  name to set
     * @param value value of constant
     * @return true
     */
    bool UpdateVariable(const std::string &name, const Value &value);

private:
    std::string        expression;
    std::ostringstream error_messages;
    // The partition of expression that the object is
    // currently working on
    std::vector<Block> blocks;

    // Vector of variables, constants and functions
    // that might be used in calculations
    std::vector<Variable> variables;
    std::vector<Variable> constants;
    std::vector<Function> functions;

    // Add predefined constants and functions
    void AddPredefined();

    void PreprocessExp();

    // This is similar to lexical analysis in a compiler
    // Partition an expression into blocks of different types
    bool GroupExp(const std::string &exp);

    // Analyze whether a std::string Block is of BlockType Func, Constant or Var
    BlockType AnalyzeStrType(const std::string &str);

    // Determine the type of one single character
    BlockType Char2Type(char c);

    // Replace every "-" as negative sign by "0-"
    void DealWithNegativeSign(std::string &exp);

    // Calculate expression in block range [startBlock,endBlock)
    Value CalculateExp(const std::string &exp, int startBlock, int endBlock);

    // Given the block id of ')', find the block id of corresponding '('
    int FindIndexOfBracketEnding(int blockId);
};
} // namespace exp_solver