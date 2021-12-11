#ifndef SPATIALPARTITION_H
#define SPATIALPARTITION_H

#include <anax/anax.hpp>
#include "../core/spatialhash.h"
#include "../core/type.h"
#include "../core/vector2.h"

namespace strat
{
namespace detail
{
  class SpatialParitionAnaxObjectHandle
  {
    using iterator_type = gengine2d::SpatialHash<anax::Entity, SpatialParitionAnaxObjectHandle>::container_type::iterator;

  public:
    SpatialParitionAnaxObjectHandle(iterator_type it);

    gengine2d::Vector2f getPosition() const;

  private:
    template<class, class> friend class ::gengine2d::SpatialHash;

    iterator_type m_iterator;
  };
}
  using spatial_partition_t = gengine2d::SpatialHash<anax::Entity,
                                                     detail::SpatialParitionAnaxObjectHandle>;
}

#endif // SPATIALPARTITION_H
