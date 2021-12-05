/*

value.cpp

Author: Jingyun Yang, SplitGemini
Date Created: 10/2/16
Modified: 4/12/2021

Description: Implementation of Value Class.

*/
#include <algorithm>
#include <cmath>
#include "value.h"

namespace exp_solver
{
using std::string;

Fraction::Fraction(int64_t u, int64_t d) : up(u), down(d) {
    // Ensure that GCD(up,down)=1
    for (int64_t k = 2; k <= std::min(std::abs(up), std::abs(down)); k++) {
        while (std::abs(up) % k == 0 && std::abs(down) % k == 0) {
            up /= k;
            down /= k;
        }
    }
}

Value::Value() :
    isDecimal(false),
    isInterger(false),
    fracValue(Fraction()),
    decValue(0.0),
    calculability(false) {}

Value::Value(Fraction fv) {
    if (fv.down == 0) {
        error_messages += "Arithmetic error: Denominator is zero! ";
        return;
    }
    isDecimal     = false;
    fracValue     = fv;
    decValue      = (double)fv.up / fv.down;
    calculability = true;
    isInterger    = (fv.down == 1 || fv.up == 0);
}

Value::Value(double dv) {
    if (dv == (double)int64_t(dv)) {
        isInterger     = true;
        fracValue.up   = (int64_t)dv;
        fracValue.down = 1;
        decValue       = dv;
        calculability  = true;
        return;
    }
    isDecimal     = true;
    fracValue     = Fraction();
    decValue      = dv;
    calculability = true;
}

Value::Value(const string &str) {
    auto found = str.find('.');
    if (found != string::npos) {
        string left  = str.substr(0, found);
        string right = str.substr(found + 1);
        if (left.length() >= 15) {
            *this = Value();
            error_messages += "Arithmetic error: Number too large! ";
            return;
        }
        size_t pos_of_not_zero;
        // remove all suffix zero
        if (std::all_of(right.begin(), right.end(), [](char c) { return c == '0'; })) {
            right.clear();
        } else if ((pos_of_not_zero = right.find_last_not_of("0")) != string::npos) {
            right.erase(pos_of_not_zero + 1);
        }

        // is interger
        if (right.length() == 0) {
            isInterger = true;
            *this      = Value(Fraction(stoi(left), 1));
            return;

        } else if (right.length() <= 5) { // use fraction
            if (right.find('.') != string::npos) {
                error_messages += "Arithmetic Error: More than one '.' in a number! ";
                Value();
                return;
            }
            int leftNumber = stoi(left), rightNumber = stoi(right);
            int multiplier = pow(10.0, (floor)(log10(rightNumber)) + 1.0);
            *this          = Value(Fraction(leftNumber * multiplier + rightNumber, multiplier));
            return;

        } else { // use raw double
            isDecimal     = true;
            fracValue     = Fraction();
            decValue      = stod(str);
            calculability = true;
            return;
        }
    }
    // normal interger
    *this      = Value(Fraction(stoi(str), 1));
    isInterger = true;
    return;
}

Value::Value(const Value &v) {
    *this = v;
}

Value &Value::operator=(const Value &v) {
    this->isDecimal      = v.isDecimal;
    this->calculability  = v.calculability;
    this->decValue       = v.decValue;
    this->fracValue      = v.fracValue;
    this->error_messages = v.error_messages;
    this->isInterger     = v.isInterger;
    return *this;
}

std::string Value::GetErrorMessage() const {
    return error_messages;
}

bool Value::IsDecimal() const {
    return isDecimal;
}

Fraction Value::GetFracValue() const {
    return fracValue;
}

bool Value::IsCalculable() const {
    return calculability;
}

string Value::GetValueStr() const {
    if (!calculability) return "";
    if (fracValue.down == 1 || fracValue.up == 0) {
        return std::to_string(fracValue.up);
    } else {
        return std::to_string(decValue);
    }
}

double Value::GetValueDouble() const {
    return decValue;
}

Value &Value::operate(const std::string &op, const Value &b) {
#if EXP_SOLVER_DEBUG
    std::cout << "value: a:" << decValue << " " << op << " b:" << b.decValue << std::endl;
#endif // EXP_SOLVER_DEBUG

    if (op == "**") {
        this->powv(b);
    } else if (op == "*") {
        *this *= b;
    } else if (op == "/") {
        *this /= b;
    } else if (op == "//") {
        *this = Value(floor((*this / b).decValue));
    } else if (op == "%") {
        *this %= b;
    } else if (op == "+") {
        *this += b;
    } else if (op == "-") {
        *this -= b;
    } else if (op == "<<") {
        *this <<= b;
    } else if (op == ">>") {
        *this >>= b;
    } else if (op == "&") {
        *this &= b;
    } else if (op == "^") {
        *this ^= b;
    } else if (op == "|") {
        *this |= b;
    } else {
        *this          = Value();
        error_messages = "Invalid operator: " + op;
    }

#if EXP_SOLVER_DEBUG
    std::cout << "result: :" << decValue << std::endl;
#endif // EXP_SOLVER_DEBUG

    return *this;
}


Value &Value::operator+=(const Value &z) {
    *this = *this + z;
    return *this;
}

Value &Value::operator-=(const Value &z) {
    *this = *this - z;
    return *this;
}

Value &Value::operator*=(const Value &z) {
    *this = *this * z;
    return *this;
}

Value &Value::operator/=(const Value &z) {
    *this = *this / z;
    return *this;
}

Value &Value::operator%=(const Value &z) {
    *this = *this % z;
    return *this;
}

Value &Value::operator<<=(const Value &z) {
    *this = *this << z;
    return *this;
}

Value &Value::operator>>=(const Value &z) {
    *this = *this >> z;
    return *this;
}

Value &Value::operator&=(const Value &z) {
    *this = *this & z;
    return *this;
}

Value &Value::operator|=(const Value &z) {
    *this = *this | z;
    return *this;
}

Value &Value::operator^=(const Value &z) {
    *this = *this ^ z;
    return *this;
}

Value &Value::powv(const Value &z) {
    *this = exp_solver::powv(*this, z);
    return *this;
}


Value operator+(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    if (a.isDecimal || b.isDecimal) {
        return Value(a.decValue + b.decValue);
    } else {
        return Value(Fraction(a.fracValue.up * b.fracValue.down + a.fracValue.down * b.fracValue.up,
                              b.fracValue.down * a.fracValue.down));
    }
}

Value operator-(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    if (a.isDecimal || b.isDecimal) {
        return Value(a.decValue - b.decValue);
    } else {
        return Value(Fraction(a.fracValue.up * b.fracValue.down - a.fracValue.down * b.fracValue.up,
                              b.fracValue.down * a.fracValue.down));
    }
}

Value operator*(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    if (a.isDecimal || b.isDecimal) {
        return Value(a.decValue * b.decValue);
    } else {
        return Value(
            Fraction(b.fracValue.up * a.fracValue.up, b.fracValue.down * a.fracValue.down));
    }
}

Value operator/(const Value &a, const Value &b) {
    if (!a.calculability || !b.IsCalculable()) { return Value(); }
    if (a.isDecimal || b.isDecimal) {
        return Value(a.decValue / b.decValue);
    } else {
        return Value(
            Fraction(b.fracValue.down * a.fracValue.up, b.fracValue.up * a.fracValue.down));
    }
}

Value operator%(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    // must both be interger
    if (a.isInterger && b.isInterger) { return Value((int64_t)a.decValue % (int64_t)b.decValue); }
    Value temp{};
    temp.error_messages = "Arithmetic error: Can't mod with float number";
    return temp;
}

Value operator<<(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    // must both be interger
    if (a.isInterger && b.isInterger && b.decValue >= 0) {
        return Value((int64_t)a.decValue << (int64_t)b.decValue);
    }
    Value temp{};
    temp.error_messages = "Arithmetic error: Can't left shift with float number or negative number";
    return temp;
}

Value operator>>(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    // must both be interger
    if (a.isInterger && b.isInterger && b.decValue >= 0) {
        return Value((int64_t)a.decValue >> (int64_t)b.decValue);
    }
    Value temp{};
    temp.error_messages =
        "Arithmetic error: Can't right shift with float number or negative number";
    return temp;
}

Value operator&(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    // must both be interger
    if (a.isInterger && b.isInterger) { return Value((int64_t)a.decValue & (int64_t)b.decValue); }
    Value temp{};
    temp.error_messages = "Arithmetic error: Can't and with float number";
    return temp;
}

Value operator|(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    // must both be interger
    if (a.isInterger && b.isInterger) { return Value((int64_t)a.decValue | (int64_t)b.decValue); }
    Value temp{};
    temp.error_messages = "Arithmetic error: Can't or with float number";
    return temp;
}

Value operator^(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    // must both be interger
    if (a.isInterger && b.isInterger) { return Value((int64_t)a.decValue ^ (int64_t)b.decValue); }
    Value temp{};
    temp.error_messages = "Arithmetic error: Can't xor with float number";
    return temp;
}

Value operator-(const Value &a) {
    return a * Value(-1.0);
}

Value operator~(const Value &a) {
    if (a.isInterger) { return Value((double)~(int64_t)a.decValue); }
    Value temp{};
    temp.error_messages = "Arithmetic error: Can't negate with float number";
    return temp;
}

Value powv(const Value &a, const Value &b) {
    if (!a.calculability || !b.calculability) { return Value(); }
    if (a.decValue < 0 && !b.isInterger) {
        Value temp{};
        temp.error_messages += "Arithmetic error: Can't power a negative number by a non-integer! ";
        return temp;
    }
    // b dec value is integer
    if (!a.isDecimal && b.isInterger) {
        Value res(a);
        res.fracValue.up   = std::pow(a.fracValue.up, std::fabs(b.decValue));
        res.fracValue.down = std::pow(a.fracValue.down, std::fabs(b.decValue));

        if (b.decValue < 0) {
            std::swap(res.fracValue.up, res.fracValue.down);
            if (res.fracValue.down < 0) {
                res.fracValue.up *= -1;
                res.fracValue.down *= -1;
            }
        }
        res.decValue   = (double)res.fracValue.up / res.fracValue.down;
        res.isInterger = res.fracValue.up == 0 || res.fracValue.down == 1;
        return res;
    } else {
        return Value(std::pow(a.decValue, b.decValue));
    }
}

std::ostream &operator<<(std::ostream &out, const Value &m) {
    return out << m.GetValueStr();
}
} // namespace exp_solver
