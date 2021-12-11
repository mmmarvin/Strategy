#ifndef SCRIPT_H
#define SCRIPT_H

#ifdef NDEBUG
#define SOL_NO_EXCEPTIONS 1
#else
#define SOL_PRINT_ERRORS 1
#endif // NDEBUG
#define SOL_ALL_SAFETIES_ON 1
#include <sol/sol.hpp>

#endif // SCRIPT_H
