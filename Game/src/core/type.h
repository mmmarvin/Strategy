#ifndef TYPE_H
#define TYPE_H

#include <array>
#include <mutex>
#include <string>

#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>

#include <EASTL/queue.h>
#include <EASTL/list.h>
#include <EASTL/set.h>
#include <EASTL/unordered_set.h>
#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>

#include <ska/unordered_map.hpp>

#include "3rdparty/fixed_sized_array.h"
#include "type_common.h"

namespace gengine2d
{
  template<class K> using fast_unordered_set_t = ska::unordered_set<K>;
  template<class K, class V> using fast_unordered_map_t = ska::unordered_map<K, V>;

  template<class K> using unordered_set_t = eastl::unordered_set<K>;
  template<class K, class V> using unordered_map_t = eastl::unordered_map<K, V>;

  using string_t = std::string;
  using wstring_t = std::wstring;

  template<class T, std::size_t Size> using fixed_vector_t = mar::container::fixed_sized_array<T, Size>;
  template<class T> using vector_t = eastl::vector<T>;
  template<class T> using list_t = eastl::list<T>;
  template<class T> using queue_t = eastl::queue<T>;

  template<class K, class V> using flat_map_t = boost::container::flat_map<K, V>;
  template<class K> using flat_set_t = boost::container::flat_set<K>;

  template<class T, std::size_t Count> using array_t = std::array<T, Count>;

  using mutex_t = std::mutex;
}

#endif // TYPE_H
