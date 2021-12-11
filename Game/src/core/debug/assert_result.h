#ifndef ASSERT_RESULT_H
#define ASSERT_RESULT_H

#include <cassert>

#ifndef NDEBUG
#define assert_result(a) assert(a)
#else
#define assert_result(a) a
#endif // NDEBUG

#endif // ASSERT_RESULT_H
