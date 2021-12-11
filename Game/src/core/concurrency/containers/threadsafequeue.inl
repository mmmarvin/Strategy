#include "threadsafequeue.h"

namespace gengine2d
{
  template<class T>
  void ThreadSafeQueue<T>::push(const T& t)
  {
    std::lock_guard<mutex_t> l(m_queueMutex);
    m_queue.push(t);
  }

  template<class T>
  void ThreadSafeQueue<T>::push(T&& t)
  {
    std::lock_guard<mutex_t> l(m_queueMutex);
    m_queue.push(std::move(t));
  }

  template<class T>
  std::unique_ptr<T> ThreadSafeQueue<T>::pop()
  {
    std::unique_lock<mutex_t> l(m_queueMutex);
    auto ret = m_queue.front();
    m_queue.pop();
    l.unlock(); // unlock since allocating memory is slow, and we don't want to hog the mutex for too long

    return std::make_unique<T>(std::move(ret));
  }

  template<class T>
  void ThreadSafeQueue<T>::pop(T& t)
  {
    std::lock_guard<mutex_t> l(m_queueMutex);
    t = std::move(m_queue.front());
    m_queue.pop();
  }

  template<class T>
  std::unique_ptr<T> ThreadSafeQueue<T>::try_pop()
  {
    std::unique_lock<mutex_t> l(m_queueMutex);
    if(!m_queue.empty()) {
      auto ret = m_queue.front();
      m_queue.pop();
      l.unlock(); // unlock since allocating memory is slow, and we don't want to hog the mutex for too long

      return std::make_unique<T>(std::move(ret));
    }

    return nullptr;
  }

  template<class T>
  bool ThreadSafeQueue<T>::try_pop(T& t)
  {
    std::lock_guard<mutex_t> l(m_queueMutex);
    if(m_queue.empty())
      return false;

    t = std::move(m_queue.front());
    m_queue.pop();
    return true;
  }

  template<class T>
  bool ThreadSafeQueue<T>::empty() const
  {
    std::lock_guard<mutex_t> l(m_queueMutex);
    return m_queue.empty();
  }

  template<class T>
  std::size_t ThreadSafeQueue<T>::size() const
  {
    std::lock_guard<mutex_t> l(m_queueMutex);
    return m_queue.size();
  }
}
