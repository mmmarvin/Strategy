#include "thread_utility.h"

#include <future>
#include "../define.h"
#include "../concurrency/thread_algorithm.h"

namespace gengine2d
{
namespace thread_utils
{
  template<class InputIteratorT,
           class Func>
  void threadPoolForEach(DeferredThreadPool& threadPool,
                         InputIteratorT first,
                         InputIteratorT last,
                         Func&& func)
  {
    // FIX: Fix the logic behind number of threads to use
    auto length = stl_utils::distance(first, last);
    if(!length)
      return;

    auto threadsCount = threadPool.getThreadsCount();
    if(!threadsCount)
      threadsCount = 1;

    auto minPerThread = THREAD_MIN_ITEM_PER_THREAD;
    auto maxThreads = (length + minPerThread - 1) / minPerThread;
    auto numberOfThreadsToUse = general_utils::min(threadsCount != 0 ? threadsCount : 2, maxThreads);

    auto blockSize = length / numberOfThreadsToUse;
    auto blockStart = first;

    for(decltype(numberOfThreadsToUse) i = 0; i < numberOfThreadsToUse; ++i) {
      auto blockEnd = blockStart;
      std::advance(blockEnd, blockSize);

      std::function<void()> forEachTask(
      [blockStart, blockEnd, &func]() {
        std::for_each(blockStart, blockEnd, func);
      });
      threadPool.addJob(i, forEachTask);
      blockStart = blockEnd;
    }
    threadPool.run();
  }
}
}
