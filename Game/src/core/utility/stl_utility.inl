#include "stl_utility.h"

#include <string>
#include <EASTL/string.h>
#include <type_traits>

namespace gengine2d
{
namespace stl_utils_extend
{
  template<class T>
  string_t to_string(T);

  template<class InputIteratorT,
           class OutputIteratorT>
  OutputIteratorT next(InputIteratorT it, typename std::iterator_traits<InputIteratorT>::difference_type n);

  template<class InputIteratorT,
           class OutputIteratorT>
  OutputIteratorT prev(InputIteratorT it, typename std::iterator_traits<InputIteratorT>::difference_type n);

  template<class InputIteratorT,
           class OutputIteratorT>
  OutputIteratorT distance(InputIteratorT first, InputIteratorT last);

  template<class InputIteratorT>
  void advance(InputIteratorT& it, typename std::iterator_traits<InputIteratorT>::difference_type n);

  template<class ContainerT,
           class OutputIteratorT,
           class = typename container_traits<ContainerT>::iterator_type,
           class = typename std::iterator_traits<typename container_traits<ContainerT>::iterator_type>::iterator_category>
  OutputIteratorT begin(ContainerT& container);

  template<class ContainerT,
           class OutputIteratorT,
           class = typename container_traits<ContainerT>::iterator_type,
           class = typename std::iterator_traits<typename container_traits<ContainerT>::iterator_type>::iterator_category>
  OutputIteratorT end(ContainerT& container);
}
namespace stl_utils
{
namespace stl_utils_detail
{
  template<class IteratorCategoryT>
  constexpr bool isStdIterator()
  {
    return std::is_same<IteratorCategoryT, std::input_iterator_tag>() ||
           std::is_same<IteratorCategoryT, std::output_iterator_tag>() ||
           std::is_same<IteratorCategoryT, std::forward_iterator_tag>() ||
           std::is_same<IteratorCategoryT, std::bidirectional_iterator_tag>() ||
           std::is_same<IteratorCategoryT, std::random_access_iterator_tag>();
  }

  template<class IteratorCategoryT>
  constexpr bool isEastlIterator()
  {
    return std::is_same<IteratorCategoryT, eastl::input_iterator_tag>() ||
           std::is_same<IteratorCategoryT, eastl::output_iterator_tag>() ||
           std::is_same<IteratorCategoryT, eastl::forward_iterator_tag>() ||
           std::is_same<IteratorCategoryT, eastl::bidirectional_iterator_tag>() ||
           std::is_same<IteratorCategoryT, eastl::random_access_iterator_tag>();
  }
}
  template<class T>
  string_t to_string(T t)
  {
    constexpr bool isSTDString = std::is_same<string_t, std::string>();
    constexpr bool isEASTLString = std::is_same<string_t, eastl::string>();
//    static_assert(isStdString || isEastlString, "Invalid StringT! Must be either std::string or eastl::string");

    if constexpr(isEASTLString) {
      return eastl::to_string(t);
    } else if constexpr(isSTDString) {
      return std::to_string(t);
    } else {
      return stl_utils_extend::to_string(t);
    }
  }

  template<class InputIteratorT>
  decltype(auto) next(InputIteratorT it, typename std::iterator_traits<InputIteratorT>::difference_type n)
  {
    using iterator_category_type = typename std::iterator_traits<InputIteratorT>::iterator_category;
    constexpr bool isSTDIterator = stl_utils_detail::isStdIterator<iterator_category_type>();
    constexpr bool isEASTLIterator = stl_utils_detail::isEastlIterator<iterator_category_type>();
//    static_assert(isSTDIterator || isEASTLIterator, "InputIteratorT must be iterator from std or eastl");

    if constexpr(isEASTLIterator) {
      return eastl::next(it, n);
    } else if constexpr(isSTDIterator) {
      return std::next(it, n);
    } else {
      return stl_utils_extend::next(it, n);
    }
  }

  template<class InputIteratorT>
  decltype(auto) prev(InputIteratorT it, typename std::iterator_traits<InputIteratorT>::difference_type n)
  {
    using iterator_category_type = typename std::iterator_traits<InputIteratorT>::iterator_category;
    constexpr bool isSTDIterator = stl_utils_detail::isStdIterator<iterator_category_type>();
    constexpr bool isEASTLIterator = stl_utils_detail::isEastlIterator<iterator_category_type>();
//    static_assert(isSTDIterator || isEASTLIterator, "InputIteratorT must be iterator from std or eastl");

    if constexpr(isEASTLIterator) {
      return eastl::prev(it, n);
    } else if constexpr(isSTDIterator) {
      return std::prev(it, n);
    } else {
      return stl_utils_extend::prev(it, n);
    }
  }

  template<class InputIteratorT>
  decltype(auto) distance(InputIteratorT first, InputIteratorT last)
  {
    using iterator_category_type = typename std::iterator_traits<InputIteratorT>::iterator_category;
    constexpr bool isSTDIterator = stl_utils_detail::isStdIterator<iterator_category_type>();
    constexpr bool isEASTLIterator = stl_utils_detail::isEastlIterator<iterator_category_type>();
//    static_assert(isSTDIterator || isEASTLIterator, "InputIteratorT must be iterator from std or eastl");

    if constexpr(isEASTLIterator) {
      return eastl::distance(first, last);
    } else if constexpr(isSTDIterator) {
      return std::distance(first, last);
    } else {
      return stl_utils_extend::distance(first, last);
    }
  }

  template<class InputIteratorT>
  void advance(InputIteratorT& it, typename std::iterator_traits<InputIteratorT>::difference_type n)
  {
    using iterator_category_type = typename std::iterator_traits<InputIteratorT>::iterator_category;
    constexpr bool isSTDIterator = stl_utils_detail::isStdIterator<iterator_category_type>();
    constexpr bool isEASTLIterator = stl_utils_detail::isEastlIterator<iterator_category_type>();
//    static_assert(isSTDIterator || isEASTLIterator, "InputIteratorT must be iterator from std or eastl");

    if constexpr(isEASTLIterator) {
      eastl::advance(it, n);
    } else if constexpr(isSTDIterator) {
      std::advance(it, n);
    } else {
      stl_utils_extend::advance(it, n);
    }
  }

  template<class ContainerT,
           class = typename container_traits<ContainerT>::iterator_type,
           class = typename std::iterator_traits<typename container_traits<ContainerT>::iterator_type>::iterator_category>
  decltype(auto) begin(ContainerT& container)
  {
    using iterator_type = typename container_traits<ContainerT>::iterator_type;
    using iterator_category_type = typename std::iterator_traits<iterator_type>::iterator_category;

    constexpr bool isSTDIterator = stl_utils_detail::isStdIterator<iterator_category_type>();
    constexpr bool isEASTLIterator = stl_utils_detail::isEastlIterator<iterator_category_type>();
//    static_assert(isSTDIterator || isEASTLIterator, "IteratorT must be iterator from std or eastl");

    if constexpr(isEASTLIterator) {
      return eastl::begin(container);
    } else if constexpr(isSTDIterator) {
      return std::begin(container);
    } else {
      return stl_utils_extend::begin(container);
    }
  }

  template<class ContainerT,
           class = typename container_traits<ContainerT>::iterator_type,
           class = typename std::iterator_traits<typename container_traits<ContainerT>::iterator_type>::iterator_category>
  decltype(auto) end(ContainerT& container)
  {
    using iterator_type = typename container_traits<ContainerT>::iterator_type;
    using iterator_category_type = typename std::iterator_traits<iterator_type>::iterator_category;

    constexpr bool isSTDIterator = stl_utils_detail::isStdIterator<iterator_category_type>();
    constexpr bool isEASTLIterator = stl_utils_detail::isEastlIterator<iterator_category_type>();
//    static_assert(isSTDIterator || isEASTLIterator, "IteratorT must be iterator from std or eastl");

    if constexpr(isEASTLIterator) {
      return eastl::end(container);
    } else if constexpr(isSTDIterator) {
      return std::end(container);
    } else {
      return stl_utils_extend::end(container);
    }
  }
}
}
