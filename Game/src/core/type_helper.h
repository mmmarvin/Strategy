#ifndef TYPE_HELPER_H
#define TYPE_HELPER_H

#include "type.h"

namespace gengine2d
{
namespace stl_utils
{
  template<class K, class V>
  decltype(auto) begin(fast_unordered_map_t<K, V>& map);

  template<class K, class V>
  decltype(auto) end(fast_unordered_map_t<K, V>& map);
}
}

#include "type_helper.inl"

#endif // TYPE_HELPER_H
