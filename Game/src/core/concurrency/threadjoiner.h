#ifndef THREADJOINER_H
#define THREADJOINER_H

namespace gengine2d
{
  template<class ContainerT>
  class ThreadJoiner
  {
  public:
    ThreadJoiner(ContainerT& threadContainer);
    ~ThreadJoiner();

  private:
    ContainerT* m_threadContainer;
  };
}

// definitions
#include "threadjoiner.inl"

#endif // THREADJOINER_H
