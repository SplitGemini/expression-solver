#define CATCH_CONFIG_MAIN
#include <cmath>

#include "catch.hpp"
#include "exp_solver.h"


TEST_CASE("Simple expression") {
    exp_solver::ExpSolver exp;
    CHECK(exp.SolveExp("1+1").GetValueDouble() == 2);
    CHECK(exp.SolveExp("1-1").GetValueDouble() == 0);
    CHECK(exp.SolveExp("-1").GetValueDouble() == -1);
    CHECK(exp.SolveExp("-1-1").GetValueDouble() == -2);
    CHECK(exp.SolveExp("-(1+1)").GetValueDouble() == -2);
    CHECK(exp.SolveExp("1*3").GetValueDouble() == 3);
    CHECK(exp.SolveExp("1/3").GetValueDouble() == Approx(1.0 / 3));
    CHECK(exp.SolveExp("4%3").GetValueDouble() == 1);
    CHECK(exp.SolveExp("7//3").GetValueDouble() == 2);
    CHECK(exp.SolveExp("2**3").GetValueDouble() == 8);
    CHECK(exp.SolveExp("1**-0.1").GetValueDouble() == Approx(std::pow(1.0, -0.1)));
    CHECK(exp.SolveExp("1&2").GetValueDouble() == 0);
    CHECK(exp.SolveExp("1|2").GetValueDouble() == 3);
    CHECK(exp.SolveExp("1^2").GetValueDouble() == 3);
    CHECK(exp.SolveExp("~1").GetValueDouble() == -2);
    CHECK(exp.SolveExp("1<<1").GetValueDouble() == 2);
    CHECK(exp.SolveExp("1>>1").GetValueDouble() == 0);
    CHECK(exp.SolveExp("3**(-1)").GetValueDouble() == Approx(1.0 / 3));
    CHECK(exp.SolveExp("sin(2*pi)").GetValueDouble() == Approx(sin(2 * M_PI)));
    CHECK(exp.SolveExp("ceil(3.14)").GetValueDouble() == 4);
    CHECK(exp.SolveExp("floor(3.14)").GetValueDouble() == 3);
    CHECK(exp.SolveExp("round(3.14)").GetValueDouble() == 3);
    CHECK(exp.SolveExp("ln(e)").GetValueDouble() == 1);
    CHECK(exp.SolveExp("exp(2)").GetValueDouble() == Approx(std::exp(2)));
    CHECK(exp.SolveExp("sqrt(9)").GetValueDouble() == 3);
    CHECK(exp.SolveExp("log(100)").GetValueDouble() == 2);
    CHECK(exp.SolveExp("abs(-2)").GetValueDouble() == 2);
}

TEST_CASE("Wrong expression") {
    exp_solver::ExpSolver exp;
    // empty
    CHECK(!exp.SolveExp("").IsCalculable());

    // wrong symbol
    CHECK(!exp.SolveExp("1++1").IsCalculable());
    CHECK(!exp.SolveExp("1---1").IsCalculable());
    CHECK(!exp.SolveExp("1<>1").IsCalculable());
    CHECK(!exp.SolveExp("-1=1").IsCalculable());
    CHECK(!exp.SolveExp("1///1").IsCalculable());
    // CHECK(!exp.SolveExp("--1/1").IsCalculable());
    CHECK(!exp.SolveExp("1$1").IsCalculable());

    // wrong variables
    CHECK(!exp.SolveExp("epsaa").IsCalculable());
    CHECK(!exp.SolveExp("ppp-1").IsCalculable());
    CHECK(!exp.SolveExp("1ii").IsCalculable());

    /// wrong rules
    CHECK(!exp.SolveExp("-1**-0.1").IsCalculable());
    CHECK(!exp.SolveExp("1.1&1").IsCalculable());
    CHECK(!exp.SolveExp("1.1|1").IsCalculable());
    CHECK(!exp.SolveExp("1.1^1").IsCalculable());
    CHECK(!exp.SolveExp("1.1<<1").IsCalculable());
    CHECK(!exp.SolveExp("1.1>>1").IsCalculable());
    CHECK(!exp.SolveExp("1>>-1").IsCalculable());
    CHECK(!exp.SolveExp("~1.1").IsCalculable());

    // incomplete expression
    CHECK(!exp.SolveExp("exp").IsCalculable());
    CHECK(!exp.SolveExp("exp()").IsCalculable());
    CHECK(!exp.SolveExp("(1+1").IsCalculable());
    CHECK(!exp.SolveExp("1+1)").IsCalculable());
    CHECK(!exp.SolveExp("1+").IsCalculable());
    CHECK(!exp.SolveExp("+1").IsCalculable());
}

TEST_CASE("Complex expression") {
    exp_solver::ExpSolver exp;

    CHECK(exp.SolveExp("1+((2-3*4)/5)**6%4").GetValueDouble() == 1);
    CHECK(exp.SolveExp("floor(ln(exp(e))+cos(2*pi))").GetValueDouble() == 3);
    // with space
    CHECK(exp.SolveExp("f l o o r ( l n ( e x p ( e ) ) + c o s (  2*  pi  ) )").GetValueDouble()
          == 3);

    // constant
    exp.AddOrEditConstant("x", 1);
    exp.AddOrEditConstant("y", 2);
    CHECK(exp.SolveExp("x+y*x/y").GetValueDouble() == 2);
    exp.AddOrEditConstant("x", 3);
    exp.AddOrEditConstant("y", 4);
    CHECK(exp.SolveExp("(x+y)*x+y").GetValueDouble() == 25);
    exp.AddOrEditConstant("x", 5);
    exp.AddOrEditConstant("y", 6);
    CHECK(exp.ResolveExp().GetValueDouble() == 61);

    exp.AddOrEditConstant("a1", 6);
    CHECK(exp.SolveExp("a1 + 1").GetValueDouble() == 7);
}

TEST_CASE("Priority expression") {
    exp_solver::ExpSolver exp;
    // '**' and '~' '-'
    CHECK(exp.SolveExp("~2**3").GetValueDouble() == -9);
    CHECK(exp.SolveExp("-2**3").GetValueDouble() == -8);

    // '~' '-' and '*' '/' '//' '%'
    CHECK(exp.SolveExp("3*-2").GetValueDouble() == -6);
    CHECK(exp.SolveExp("3*~2").GetValueDouble() == -9);
    CHECK(exp.SolveExp("3/-2").GetValueDouble() == Approx(3.0 / -2));
    CHECK(exp.SolveExp("3//-2").GetValueDouble() == -2);
    CHECK(exp.SolveExp("3%-2").GetValueDouble() == 1);

    // '*' '/' '//' '%' and '+' '-'
    CHECK(exp.SolveExp("3-2*4").GetValueDouble() == -5);
    CHECK(exp.SolveExp("3-2*4-1/8+9%2+1").GetValueDouble() == Approx(-3.125));

    // '+' '-' and '<<' '>>'
    CHECK(exp.SolveExp("3<<2+4").GetValueDouble() == 192);
    CHECK(exp.SolveExp("15>>1+1").GetValueDouble() == 3);

    // '<<' '>>' and '&'
    CHECK(exp.SolveExp("1&1<<1").GetValueDouble() == 0);
    // '&' and '^'
    CHECK(exp.SolveExp("5^2&3").GetValueDouble() == 7);
    // '^' and '|'
    CHECK(exp.SolveExp("5|2^3").GetValueDouble() == 5);

    // combine, from low to high
    CHECK(exp.SolveExp("5|2^3<<2+2*2**2").GetValueDouble() == 3079);
}
