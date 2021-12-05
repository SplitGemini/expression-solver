# Introduction

Expression Solver is a C++ program that solves algebraic expressions.

## examples

```c++
// new exp solver
exp_solver::ExpSoler exp;
// calculate simple expression
auto output = exp.SolveExp("1+1") // will be 2
// symbol like python, 
// see https://docs.python.org/zh-cn/3.7/reference/expressions.html#operator-precedence
// Calculate basic expressions using symbols '+', '-', '*', '/', '//', '**', '%', 
// bit operator '|', '&', '^', '<<', '>>', 
// brackets '(', ')' and numbers
// priority high to low:  { "**" }, { "~" }, { "*", "/", "//", "%" }, { "+", "-" }, 
// { "<<", ">>" }, { "&" }, { "^" },  { "|" }
output = exp.SolveExp("1+((2-3*4)/5)**6%4") // will be 1
// Maintain values in fractions until non-fractions break in
output = exp.SolveExp("0.5+1/3") // will be 0.833333
// Recognize constants "pi", "e" and 
// functions "sin", "cos", "tan", "exp", "sqrt", "floor", "ceil", "round", "ln", "log", "abs"
output = exp.SolveExp("floor(ln(exp(e))+cos(2*pi))") // will be 3

// use constant value
exp.AddOrEditConstant("x", 10);
exp.AddOrEditConstant("y", "20");
output = exp.SolveExp("x+y") // will be 30

// edit constant
exp.AddOrEditConstant("x", 20);
exp.AddOrEditConstant("y", "30");
output = exp.ResolveExp() // will be 50

// use old value
exp.AddOrEditConstant("old_value", output);
output = exp.SolveExp("old_value - 10") // will be 40


// Expression validation

output = exp.SolveExp("undefined") // wil be empty
error = exp.GetErrorMessages() // `String "undefined" not recognized!`

output = exp.SolveExp("sqrt()") // wil be empty
error = exp.GetErrorMessages() // `Invalid expression!`

// Arithmetic errors
output = exp.SolveExp("10/(floor(pi)-3)") // wil be empty
error = exp.GetErrorMessages() // `Arithmetic error: Denominator is zero!`  

output = exp.SolveExp("-1^1.5") // wil be empty
error = exp.GetErrorMessages() // `Arithmetic error: Can't power a negative number by a non-integer!`

output = exp.SolveExp("sqrt(-1)") // wil be empty 
error = exp.GetErrorMessages() // `Arithmetic error: Cannot square root a negative number!`

// Syntax errors
output = exp.SolveExp("2=2=2") // wil be empty
error = exp.GetErrorMessages() // `Syntax error: Too many '='!`  

output = exp.SolveExp("sqrt(exp(2+log(10))") // wil be empty
error = exp.GetErrorMessages() // `Syntax error: Brackets not paired!`  

output = exp.SolveExp("sqrt+3") // wil be empty
error = exp.GetErrorMessages() // `Syntax Error: Need brackets after function name!`
```

## Limitations

Precision error
> Example: `sin(pi*2)`  
> Output: `Ans = 0.000001`

Out-of-bound error
> Example: `10^18`  
> Output: `-2147483648`

Any other error that I happen to miss
> Example: `<Some magical expression>`  
> ErrorMessages: `<Some magical output>`

## Notes

Input is case sensitive
> Example: `Pi`  
> ErrorMessages: `String "Pi" not recognized!`

Spaces are ignored
> Example: `f l o o r ( 3. 1 4)`  
> Output: `3`

Negative priority is larger than **
> Example: `-2**4`
> Expect by python: -16  
> But here output is: 16

`--` symbol will be valid
> Example: `--1+1`
> Will be output: `2`  
> Because it will be replace to `0-(0-1)+1` first

## Use in Cmake

add source to your project and add `target_link_libraries(exp_solver_test PRIVATE libexp_solver)`  
to your CMakeLists.txt to add library(default is static library)

## Use

some examples and a simple main file can be found at tests dir

## Unittest

see tests/unit_tests.cpp
pass several unittest, more cases can be added by yourself.
