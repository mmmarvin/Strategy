#ifndef SPATIALHASH_H
#define SPATIALHASH_H

#include <boost/unordered_map.hpp>
#include "type.h"

namespace gengine2d
{
  template<class, class> class SpatialHash;
namespace detail
{
  template<class T>
  class SpatialHashDefaultObjectHandle
  {
    using iterator_type = typename boost::unordered_multimap<std::pair<float, float>, T>::iterator;

  public:
    SpatialHashDefaultObjectHandle(iterator_type iterator);

  private:
    template<class, class> friend class ::gengine2d::SpatialHash;

    iterator_type m_iterator;
  };
}
  template<class T,
           class ObjectHandleT = detail::SpatialHashDefaultObjectHandle<T>>
  class SpatialHash
  {
  public:
    using container_type = boost::unordered_multimap<std::pair<float, float>, T>;

    SpatialHash() {}

    ObjectHandleT add(float x, float y, const T& t);
    ObjectHandleT add(float x, float y, T&& t);
    ObjectHandleT addFloored(float x, float y, const T& t);
    ObjectHandleT addFloored(float x, float y, T&& t);

    void clear();

    void remove(ObjectHandleT handle);

    template<class Func> void forEach(float minX,
                                      float minY,
                                      float maxX,
                                      float maxY,
                                      Func func,
                                      float xc = 1.f,
                                      float yc = 1.f);
    template<class Func> void forEach(float minX,
                                      float minY,
                                      float maxX,
                                      float maxY,
                                      Func func,
                                      float xc = 1.f,
                                      float yc = 1.f) const;

    template<class Func> std::pair<bool, T*> findIf(float minX,
                                                    float minY,
                                                    float maxX,
                                                    float maxY,
                                                    Func func,
                                                    float xc = 1.f,
                                                    float yc = 1.f);
    template<class Func> std::pair<bool, const T*> findIf(float minX,
                                                          float minY,
                                                          float maxX,
                                                          float maxY,
                                                          Func func,
                                                          float xc = 1.f,
                                                          float yc = 1.f) const;

    vector_t<T*> getSingle(float x, float y);
    vector_t<const T*> getSingle(float x, float y) const ;

    vector_t<T*> getAll(float minX,
                        float minY,
                        float maxX,
                        float maxY,
                        float xc = 1.f,
                        float yc = 1.f);
    vector_t<const T*> getAll(float minX,
                              float minY,
                              float maxX,
                              float maxY,
                              float xc = 1.f,
                              float yc = 1.f) const;

    template<class Func> vector_t<T*> getIf(float minX,
                                            float minY,
                                            float maxX,
                                            float maxY,
                                            Func func,
                                            float xc = 1.f,
                                            float yc = 1.f);
    template<class Func> vector_t<const T*> getIf(float minX,
                                                  float minY,
                                                  float maxX,
                                                  float maxY,
                                                  Func func,
                                                  float xc = 1.f,
                                                  float yc = 1.f) const;

  private:
    container_type m_spatialHash;
  };
}

// definitions
#include "spatialhash.inl"

#endif // SPATIALHASH_H
