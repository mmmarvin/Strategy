#ifndef THREAD_UTILITY_H
#define THREAD_UTILITY_H

#include "../concurrency/deferredthreadpool.h"

namespace gengine2d
{
namespace thread_utils
{
  template<class InputIteratorT, class Func>
  void threadPoolForEach(DeferredThreadPool& threadPool,
                         InputIteratorT first,
                         InputIteratorT last,
                         Func&& func);
}
}

#include "thread_utility.inl"

#endif // THREAD_UTILITY_H
