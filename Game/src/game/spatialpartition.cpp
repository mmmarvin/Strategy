#include "spatialpartition.h"

#include <cmath>
#include "../core/utility/float_utility.h"
#include "entity/component/base_components.h"
#include "game_def.h"

namespace strat
{
namespace detail
{
  SpatialParitionAnaxObjectHandle::SpatialParitionAnaxObjectHandle(iterator_type it) :
    m_iterator(it)
  {
  }

  gengine2d::Vector2f SpatialParitionAnaxObjectHandle::getPosition() const
  {
    const auto& v = m_iterator->first;
    return { v.first, v.second };
  }
}
}
