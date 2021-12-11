#ifndef SINGLETON_H
#define SINGLETON_H

#include <cassert>

namespace gengine2d
{
  template<class T>
  class Singleton
  {
  public:
    static T& getInstance()
    {
      static T instance;

      return instance;
    }
  };

  template<class T>
  struct InitializeableSingleton
  {
    template<class... Args>
    static void init(Args&&... args)
    {
      auto** instance = getInstanceImpl();
      assert(!*instance);

      static T t(std::forward<Args>(args)...);
      *instance = &t;
    }

    static T& getInstance()
    {
      auto** instance = getInstanceImpl();
      assert(*instance);

      return **instance;
    }

  private:
    static T** getInstanceImpl()
    {
      static T* instance = 0;
      return &instance;
    }
  };
}

#endif // SINGLETON_H
