/*

value.h

Author: Jingyun Yang, SplitGemini
Date Created: 10/2/16
Modified: 4/12/2021

Description: Header file for arithmetic type
Value that stores numbers represented in either
fraction or decimal format.

*/
#pragma once
#include <iostream>
#include <string>

#if __cplusplus >= 201703L
#include <string_view>
#include <charconv>
#endif

#ifndef EXP_SOLVER_DEBUG
#define EXP_SOLVER_DEBUG 0
#endif // !EXP_SOLVER_DEBUG

namespace exp_solver
{
struct Fraction {
    int64_t up, down;
    Fraction() : up(0), down(1) {}
    Fraction(int64_t u, int64_t d);
};

class Value {
public:
    // Constructors
    Value();
    // copy contructor
    Value(const Value &);
    Value &operator=(const Value &z);

#if __cplusplus >= 201703L
    Value(const std::string_view &str);
#endif
    Value(const std::string &str);
    Value(Fraction fv);
    Value(double dv);

    // Check properties
    bool IsDecimal() const;
    bool IsCalculable() const;
    // Getters
    Fraction    GetFracValue() const;
    std::string GetErrorMessage() const;
    std::string GetValueStr() const;
    double      GetValueDouble() const;


    // Operator overload
    Value &operator+=(const Value &z);
    Value &operator-=(const Value &z);
    Value &operator*=(const Value &z);
    Value &operator/=(const Value &z);
    Value &operator%=(const Value &z);
    Value &operator<<=(const Value &z);
    Value &operator>>=(const Value &z);
    Value &operator&=(const Value &z);
    Value &operator|=(const Value &z);
    Value &operator^=(const Value &z);
    Value &powv(const Value &z);


    friend Value operator+(const Value &a, const Value &b);
    friend Value operator-(const Value &a, const Value &b);
    friend Value operator*(const Value &a, const Value &b);
    friend Value operator/(const Value &a, const Value &b);
    friend Value operator%(const Value &a, const Value &b);
    friend Value operator<<(const Value &a, const Value &b);
    friend Value operator>>(const Value &a, const Value &b);
    friend Value operator&(const Value &a, const Value &b);
    friend Value operator|(const Value &a, const Value &b);
    friend Value operator^(const Value &a, const Value &b);
    friend Value operator-(const Value &a);
    friend Value operator~(const Value &a);
    friend Value powv(const Value &a, const Value &b);

    friend std::ostream &operator<<(std::ostream &out, const Value &m);

private:
    friend class ExpSolver;
#if __cplusplus >= 201703L
    Value &operate(const std::string_view &op, const Value &b);
#endif
    Value &     operate(const std::string &op, const Value &b);
    bool        isDecimal;
    bool        isInterger;
    Fraction    fracValue;
    double      decValue;
    bool        calculability;
    std::string error_messages;
};

Value operator+(const Value &a, const Value &b);
Value operator-(const Value &a, const Value &b);
Value operator*(const Value &a, const Value &b);
Value operator/(const Value &a, const Value &b);
Value operator%(const Value &a, const Value &b);
Value operator<<(const Value &a, const Value &b);
Value operator>>(const Value &a, const Value &b);
Value operator&(const Value &a, const Value &b);
Value operator|(const Value &a, const Value &b);
Value operator^(const Value &a, const Value &b);
Value operator-(const Value &a);
Value operator~(const Value &a);
Value powv(const Value &a, const Value &b);

std::ostream &operator<<(std::ostream &out, const Value &m);

} // namespace exp_solver