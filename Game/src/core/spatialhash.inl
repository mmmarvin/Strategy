#include "spatialhash.h"

#include <cmath>
#include "debug/debug_log.h"
#include "utility/stl_utility.h"

namespace gengine2d
{
namespace detail
{
  template<class T>
  SpatialHashDefaultObjectHandle<T>::SpatialHashDefaultObjectHandle(iterator_type iterator) :
    m_iterator(iterator)
  {
  }
}
  template<class T, class ObjectHandleT>
  ObjectHandleT SpatialHash<T, ObjectHandleT>::add(float x, float y, const T& t)
  {
    auto fx = floorf(x);
    auto fy = floorf(y);

//        return m_spatialHash.emplace(std::make_pair(fx, fy), t);
    return addFloored(fx, fy, t);
  }

  template<class T, class ObjectHandleT>
  ObjectHandleT SpatialHash<T, ObjectHandleT>::add(float x, float y, T&& t)
  {
    auto fx = floorf(x);
    auto fy = floorf(y);

//        return m_spatialHash.emplace(std::make_pair(fx, fy), std::move(t));
    return addFloored(fx, fy, std::move(t));
  }

  template<class T, class ObjectHandleT>
  ObjectHandleT SpatialHash<T, ObjectHandleT>::addFloored(float x, float y, const T& t)
  {
    auto ret = m_spatialHash.emplace(std::make_pair(x, y), t);
    return ret;
  }

  template<class T, class ObjectHandleT>
  ObjectHandleT SpatialHash<T, ObjectHandleT>::addFloored(float x, float y, T&& t)
  {
    auto ret = m_spatialHash.emplace(std::make_pair(x, y), std::move(t));
    return ret;
  }

  template<class T, class ObjectHandleT>
  void SpatialHash<T, ObjectHandleT>::clear()
  {
    m_spatialHash.clear();
  }

  template<class T, class ObjectHandleT>
  void SpatialHash<T, ObjectHandleT>::remove(ObjectHandleT handle)
  {
    m_spatialHash.erase(handle.m_iterator);
  }

  template<class T, class ObjectHandleT>
  template<class Func>
  void SpatialHash<T, ObjectHandleT>::forEach(float minX,
                                              float minY,
                                              float maxX,
                                              float maxY,
                                              Func func,
                                              float xc,
                                              float yc)
  {
    auto fminX = floorf(minX);
    auto fminY = floorf(minY);
    auto fmaxX = floorf(maxX);
    auto fmaxY = floorf(maxY);
    for(auto y = fminY; y <= fmaxY; y += yc) {
      for(auto x = fminX; x <= fmaxX; x += xc) {
        auto range = m_spatialHash.equal_range(std::make_pair(x, y));
        for(; range.first != range.second; ++range.first) {
          func(range.first->second);
        }
      }
    }
  }

  template<class T, class ObjectHandleT>
  template<class Func>
  void SpatialHash<T, ObjectHandleT>::forEach(float minX,
                                              float minY,
                                              float maxX,
                                              float maxY,
                                              Func func,
                                              float xc,
                                              float yc) const
  {
    auto fminX = floorf(minX);
    auto fminY = floorf(minY);
    auto fmaxX = floorf(maxX);
    auto fmaxY = floorf(maxY);
    for(auto y = fminY; y <= fmaxY; y += yc) {
      for(auto x = fminX; x <= fmaxX; x += xc) {
        auto range = m_spatialHash.equal_range(std::make_pair(x, y));
        for(; range.first != range.second; ++range.first) {
          func(range.first->second);
        }
      }
    }
  }

  template<class T, class ObjectHandleT>
  template<class Func>
  std::pair<bool, T*> SpatialHash<T, ObjectHandleT>::findIf(float minX,
                                                            float minY,
                                                            float maxX,
                                                            float maxY,
                                                            Func func,
                                                            float xc,
                                                            float yc)
  {
    auto fminX = floorf(minX);
    auto fminY = floorf(minY);
    auto fmaxX = floorf(maxX);
    auto fmaxY = floorf(maxY);
    for(auto y = fminY; y <= fmaxY; y += yc) {
      for(auto x = fminX; x <= fmaxX; x += xc) {
        auto range = m_spatialHash.equal_range(std::make_pair(x, y));
        for(; range.first != range.second; ++range.first) {
          auto& o = range.first->second;

          if(func(o))
            return std::make_pair(true, &o);
        }
      }
    }

    return std::make_pair(false, nullptr);
  }

  template<class T, class ObjectHandleT>
  template<class Func>
  std::pair<bool, const T*> SpatialHash<T, ObjectHandleT>::findIf(float minX,
                                                                  float minY,
                                                                  float maxX,
                                                                  float maxY,
                                                                  Func func,
                                                                  float xc,
                                                                  float yc) const
  {
    auto fminX = floorf(minX);
    auto fminY = floorf(minY);
    auto fmaxX = floorf(maxX);
    auto fmaxY = floorf(maxY);
    for(auto y = fminY; y <= fmaxY; y += yc) {
      for(auto x = fminX; x <= fmaxX; x += xc) {
        auto range = m_spatialHash.equal_range(std::make_pair(x, y));
        for(; range.first != range.second; ++range.first) {
          auto& o = range.first->second;

          if(func(o))
            return std::make_pair(true, &o);
        }
      }
    }

    return std::make_pair(false, nullptr);
  }

  template<class T, class ObjectHandleT>
  vector_t<T*> SpatialHash<T, ObjectHandleT>::getSingle(float x, float y)
  {
    vector_t<T*> ret;

    auto range = m_spatialHash.equal_range(std::make_pair(x, y));
    for(; range.first != range.second; ++range.first) {
      ret.push_back(&range.first->second);
    }

    return ret;
  }

  template<class T, class ObjectHandleT>
  vector_t<const T*> SpatialHash<T, ObjectHandleT>::getSingle(float x, float y) const
  {
    vector_t<const T*> ret;

    auto range = m_spatialHash.equal_range(std::make_pair(x, y));
    for(; range.first != range.second; ++range.first) {
      ret.push_back(&range.first->second);
    }

    return ret;
  }

  template<class T, class ObjectHandleT>
  vector_t<T*> SpatialHash<T, ObjectHandleT>::getAll(float minX,
                                                     float minY,
                                                     float maxX,
                                                     float maxY,
                                                     float xc,
                                                     float yc)
  {
    vector_t<T*> ret;

    auto fminX = floorf(minX);
    auto fminY = floorf(minY);
    auto fmaxX = floorf(maxX);
    auto fmaxY = floorf(maxY);
    for(auto y = fminY; y <= fmaxY; y += yc) {
      for(auto x = fminX; x <= fmaxX; x += xc) {
        auto range = m_spatialHash.equal_range(std::make_pair(x, y));

        ret.reserve(ret.capacity() + stl_utils::distance(range.first, range.second));
        for(; range.first != range.second; ++range.first)
          ret.push_back(&(range.first->second));
      }
    }

    return ret;
  }

  template<class T, class ObjectHandleT>
  vector_t<const T*> SpatialHash<T, ObjectHandleT>::getAll(float minX,
                                                           float minY,
                                                           float maxX,
                                                           float maxY,
                                                           float xc,
                                                           float yc) const
  {
    vector_t<const T*> ret;

    auto fminX = floorf(minX);
    auto fminY = floorf(minY);
    auto fmaxX = floorf(maxX);
    auto fmaxY = floorf(maxY);
    for(auto y = fminY; y <= fmaxY; y += yc) {
      for(auto x = fminX; x <= fmaxX; x += xc) {
        auto range = m_spatialHash.equal_range(std::make_pair(x, y));

        ret.reserve(ret.capacity() + stl_utils::distance(range.first, range.second));
        for(; range.first != range.second; ++range.first)
          ret.push_back(&(range.first->second));
      }
    }

    return ret;
  }

  template<class T, class ObjectHandleT>
  template<class Func>
  vector_t<T*> SpatialHash<T, ObjectHandleT>::getIf(float minX,
                                                    float minY,
                                                    float maxX,
                                                    float maxY,
                                                    Func func,
                                                    float xc,
                                                    float yc)
  {
    vector_t<T*> ret;

    auto fminX = floorf(minX);
    auto fminY = floorf(minY);
    auto fmaxX = floorf(maxX);
    auto fmaxY = floorf(maxY);
    for(auto y = fminY; y <= fmaxY; y += yc) {
      for(auto x = fminX; x <= fmaxX; x += xc) {
        auto range = m_spatialHash.equal_range(std::make_pair(x, y));

        ret.reserve(ret.capacity() + stl_utils::distance(range.first, range.second));
        for(; range.first != range.second; ++range.first) {
          auto& o = range.first->second;
          if(func(o))
            ret.push_back(&o);
        }
      }
    }

    return ret;
  }

  template<class T, class ObjectHandleT>
  template<class Func>
  vector_t<const T*> SpatialHash<T, ObjectHandleT>::getIf(float minX,
                                                          float minY,
                                                          float maxX,
                                                          float maxY,
                                                          Func func,
                                                          float xc,
                                                          float yc) const
  {
    vector_t<const T*> ret;

    auto fminX = floorf(minX);
    auto fminY = floorf(minY);
    auto fmaxX = floorf(maxX);
    auto fmaxY = floorf(maxY);
    for(auto y = fminY; y <= fmaxY; y += yc) {
      for(auto x = fminX; x <= fmaxX; x += xc) {
        auto range = m_spatialHash.equal_range(std::make_pair(x, y));

        ret.reserve(ret.capacity() + stl_utils::distance(range.first, range.second));
        for(; range.first != range.second; ++range.first) {
          auto& o = range.first->second;
          if(func(o))
            ret.push_back(&o);
        }
      }
    }

    return ret;
  }
}
