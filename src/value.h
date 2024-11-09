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
#include <cstdint>
#include "exp_config.h"

namespace exp_solver
{
struct Fraction {
    int64_t up{}, down{};
    Fraction() : up(0), down(1){};
    Fraction(int64_t u, int64_t d);

    Fraction(const Fraction &)            = default;
    Fraction &operator=(const Fraction &) = default;
    Fraction(Fraction &&)                 = default;
    Fraction &operator=(Fraction &&)      = default;
};

class Value {
public:
    // Constructors
    Value() = default;
    // copy contructor
    Value(const Value &)            = default;
    Value &operator=(const Value &) = default;
    Value(Value &&)                 = default;
    Value &operator=(Value &&)      = default;

#if defined(EXP_HAS_STRING_VIEW)
    explicit Value(const std::string_view &str);
#endif
    explicit Value(const std::string &str);
    explicit Value(Fraction fv);
    explicit Value(double dv);

    template <typename T>
    inline Value(T dv) {
        static_assert(std::is_same<int32_t, T>() || std::is_same<uint32_t, T>()
                          || std::is_same<int64_t, T>() || std::is_same<uint64_t, T>()
                          || std::is_same<uint16_t, T>() || std::is_same<int16_t, T>(),
                      "unsupported type");
                      
        isInterger    = true;
        isDecimal     = true;
        fracValue     = Fraction();
        decValue      = static_cast<double>(dv);
        calculability = true;
    }

    // Check properties
    bool IsDecimal() const;
    bool IsCalculable() const;
    // Getters
    Fraction    GetFracValue() const;
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

#if defined(EXP_HAS_STRING_VIEW)
    Value &operate(std::string_view op, const Value &b);
#endif

    void fractionInit(const Fraction &);
    void doubelInit(double);

    Value &operate(const std::string &op, const Value &b);

    std::string GetErrorMessage() const;

    bool        isDecimal{ false };
    bool        isInterger{ false };
    Fraction    fracValue{};
    double      decValue{ 0 };
    bool        calculability{ false };
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