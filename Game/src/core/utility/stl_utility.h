#ifndef STL_UTILITY_H
#define STL_UTILITY_H

#include "../type_traits.h"
#include "../type.h"

namespace gengine2d
{
namespace stl_utils
{
  template<class T>
  string_t to_string(T t);

  template<class InputIteratorT>
  decltype(auto) next(InputIteratorT it, typename std::iterator_traits<InputIteratorT>::difference_type i);
  template<class InputIteratorT>
  decltype(auto) prev(InputIteratorT it, typename std::iterator_traits<InputIteratorT>::difference_type i);
  template<class InputIteratorT>
  decltype(auto) distance(InputIteratorT first, InputIteratorT last);

  template<class ContainerT,
           class = typename container_traits<ContainerT>::iterator_type,
           class = typename std::iterator_traits<typename container_traits<ContainerT>::iterator_type>::iterator_category>
  decltype(auto) begin(ContainerT& container);
  template<class ContainerT,
           class = typename container_traits<ContainerT>::iterator_type,
           class = typename std::iterator_traits<typename container_traits<ContainerT>::iterator_type>::iterator_category>
  decltype(auto) end(ContainerT& container);
}
}

#include "stl_utility.inl"

#endif // STL_UTILITY_H
