#ifndef RESOURCELOOKUP_H
#define RESOURCELOOKUP_H

#include "type.h"
#include "type_traits.h"

namespace gengine2d
{
namespace detail
{
  template<class T>
  struct ResourceLookupDefaultDeleter
  {
    void operator()(T&) {}
  };
}
  template<class T,
           class DeleterT = detail::ResourceLookupDefaultDeleter<T>>
  class ResourceLookup
  {
  public:
    ResourceLookup() {}

    void clear();

    bool exists(const string_t& name) const;
    std::size_t size() const;

    bool addResource(string_t name, T&& resource);
    bool removeResource(const string_t& name);
    template<class CustomDeleterT> bool removeResource(const string_t& name, CustomDeleterT&& customDeleter);

    T& getResource(const string_t& name);
    const T& getResource(const string_t& name) const;

    T& getResourceByIndex(std::size_t index);
    const T& getResourceByIndex(std::size_t index) const;

  private:
    template<class InputIteratorT, class CustomDeleterT> void removeResourceByIterator(InputIteratorT it,
                                                                                       CustomDeleterT&& customDeleter);

    fast_unordered_map_t<string_t, T> m_resources;
  };
}

#include "resourcelookup.inl"

#endif // RESOURCELOOKUP_H
