#ifndef TYPE_TRAITS_H
#define TYPE_TRAITS_H

#include <list>
#include <queue>
#include <EASTL/string.h>
#include "type.h"

namespace gengine2d
{
  template<class ContainerT>
  struct container_traits;

  template<class... Args>
  struct container_traits<std::initializer_list<Args...>>
  {
    using iterator_type = typename std::initializer_list<Args...>::iterator;
  };

  template<class... Args>
  struct container_traits<eastl::unordered_set<Args...>>
  {
    using iterator_type = typename eastl::unordered_set<Args...>::iterator;
  };

  template<class... Args>
  struct container_traits<eastl::unordered_map<Args...>>
  {
    using iterator_type = typename eastl::unordered_map<Args...>::iterator;
  };

  template<>
  struct container_traits<std::string>
  {
    using iterator_type = std::string::iterator;
  };

  template<>
  struct container_traits<eastl::string>
  {
    using iterator_type = eastl::string::iterator;
  };

  template<class T, std::size_t Size>
  struct container_traits<mar::container::fixed_sized_array<T, Size>>
  {
    using iterator_type = typename mar::container::fixed_sized_array<T, Size>::iterator;
  };

  template<class... Args>
  struct container_traits<std::vector<Args...>>
  {
    using iterator_type = typename std::vector<Args...>::iterator;
  };

  template<class... Args>
  struct container_traits<eastl::vector<Args...>>
  {
    using iterator_type = typename eastl::vector<Args...>::iterator;
  };

  template<class... Args>
  struct container_traits<std::list<Args...>>
  {
    using iterator_type = typename std::list<Args...>::iterator;
  };

  template<class... Args>
  struct container_traits<eastl::list<Args...>>
  {
    using iterator_type = typename eastl::list<Args...>::iterator;
  };

  template<class... Args>
  struct container_traits<std::queue<Args...>>
  {
    using iterator_type = typename std::queue<Args...>::iterator;
  };

  template<class... Args>
  struct container_traits<eastl::queue<Args...>>
  {
    using iterator_type = typename eastl::queue<Args...>::iterator;
  };

  template<class... Args>
  struct container_traits<boost::container::flat_map<Args...>>
  {
    using iterator_type = typename boost::container::flat_map<Args...>::iterator;
  };

  template<class... Args>
  struct container_traits<boost::container::flat_set<Args...>>
  {
    using iterator_type = typename boost::container::flat_set<Args...>::iterator;
  };

  template<class T, std::size_t Size>
  struct container_traits<std::array<T, Size>>
  {
    using iterator_type = typename std::array<T, Size>::iterator;
  };
}

#endif // TYPE_TRAITS_H
