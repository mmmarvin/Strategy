#include "type_helper.h"

namespace gengine2d
{
namespace stl_utils
{
  template<class K, class V>
  decltype(auto) begin(fast_unordered_map_t<K, V>& map)
  {
    return map.begin();
  }

  template<class K, class V>
  decltype(auto) end(fast_unordered_map_t<K, V>& map)
  {
    return map.end();
  }
}
}
