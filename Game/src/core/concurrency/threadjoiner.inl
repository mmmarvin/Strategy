#include "threadjoiner.h"

namespace gengine2d
{
  template<class ContainerT>
  ThreadJoiner<ContainerT>::ThreadJoiner(ContainerT& threadContainer) :
    m_threadContainer(&threadContainer)
  {
  }

  template<class ContainerT>
  ThreadJoiner<ContainerT>::~ThreadJoiner()
  {
    for(auto& thread : *m_threadContainer) {
      if(thread.joinable()) {
        thread.join();
      }
    }
  }
}
