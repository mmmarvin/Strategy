#include "resourcelookup.h"

#include "utility/stl_utility.h"
#include "type_helper.h"

namespace gengine2d
{
  template<class T, class DeleterT>
  void ResourceLookup<T, DeleterT>::clear()
  {
    for(auto it = stl_utils::begin(m_resources), itEnd = stl_utils::end(m_resources); it != itEnd; ++it) {
      removeResourceByIterator(it, DeleterT());
    }
  }

  template<class T, class DeleterT>
  bool ResourceLookup<T, DeleterT>::exists(const string_t& name) const
  {
    return (m_resources.find(name) != m_resources.end());
  }

  template<class T, class DeleterT>
  std::size_t ResourceLookup<T, DeleterT>::size() const
  {
    return m_resources.size();
  }

  template<class T, class DeleterT>
  bool ResourceLookup<T, DeleterT>::addResource(string_t name, T&& resource)
  {
    auto res = m_resources.emplace(name, std::move(resource));
    return res.second;
  }

  template<class T, class DeleterT>
  bool ResourceLookup<T, DeleterT>::removeResource(const string_t& name)
  {
    auto findIt = m_resources.find(name);
    if(findIt != m_resources.end()) {
      removeResourceByIterator(findIt, DeleterT());
      return true;
    }

    return false;
  }

  template<class T, class DeleterT>
  template<class CustomDeleterT>
  bool ResourceLookup<T, DeleterT>::removeResource(const string_t& name, CustomDeleterT&& customDeleter)
  {
    auto findIt = m_resources.find(name);
    if(findIt != m_resources.end()) {
      removeResourceByIterator(findIt, std::forward<CustomDeleterT>(customDeleter));
      return true;
    }

    return false;
  }

  template<class T, class DeleterT>
  T& ResourceLookup<T, DeleterT>::getResource(const string_t& name)
  {
    auto findIt = m_resources.find(name);
    assert(findIt != m_resources.end());

    return findIt->second;
  }

  template<class T, class DeleterT>
  const T& ResourceLookup<T, DeleterT>::getResource(const string_t& name) const
  {
    auto findIt = m_resources.find(name);
    assert(findIt != m_resources.end());

    return findIt->second;
  }

  template<class T, class DeleterT>
  T& ResourceLookup<T, DeleterT>::getResourceByIndex(std::size_t index)
  {
    auto it = stl_utils::next(m_resources.begin(), index);
    return it->second;
  }

  template<class T, class DeleterT>
  const T& ResourceLookup<T, DeleterT>::getResourceByIndex(std::size_t index) const
  {
    auto it = stl_utils::next(m_resources.begin(), index);
    return it->second;
  }

  template<class T, class DeleterT>
  template<class InputIteratorT, class CustomDeleterT>
  void ResourceLookup<T, DeleterT>::removeResourceByIterator(InputIteratorT it,
                                                             CustomDeleterT&& customDeleter)
  {
    std::forward<CustomDeleterT>(customDeleter)(it->second);
    m_resources.erase(it);
  }
}
