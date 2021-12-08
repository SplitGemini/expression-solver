#pragma once

// string view config
#ifndef EXP_HAS_STRING_VIEW

#if __cplusplus >= 201703L

#if defined __has_include

#if __has_include(<charconv>)

#include <string_view>
#include <charconv>
#define EXP_HAS_STRING_VIEW

#endif // __has_include (<charconv>)

#endif // defined __has_include

#endif // __cplusplus >= 201703L

#endif // EXP_HAS_STRING_VIEW


/// debug config
#ifndef EXP_SOLVER_DEBUG

#define EXP_SOLVER_DEBUG 1

#endif // !EXP_SOLVER_DEBUG
