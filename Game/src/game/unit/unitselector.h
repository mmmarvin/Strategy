#ifndef UNITSELECTORSYSTEM_H
#define UNITSELECTORSYSTEM_H

#include <anax/anax.hpp>
#include "../../core/3rdparty/high_resolution_clock.h"
#include "../../core/game_internal_types.h"
#include "../../core/type.h"
#include "../../core/vector2.h"
#include "../entity/component/base_components.h"
#include "../player.h"
#include "../spatialpartition.h"

namespace strat
{
  class UnitSelector
  {
  public:
    UnitSelector(const spatial_partition_t& spatialPartition);

    void draw(bool showDebugSelectionGrid);

    anax::Entity selectEntity(const gengine2d::Vector2f& position);
    gengine2d::vector_t<anax::Entity> selectEntities(Player::EPlayerNumber team,
                                                     const gengine2d::Vector2f& isoSelectionStart,
                                                     const gengine2d::Vector2f& isoSelectionEnd);

  private:
    std::array<gengine2d::Vertex2, 6>   m_selectionVertex;
    mar::high_resolution_stopwatch      m_selectionTimer;

    const spatial_partition_t*          m_spatialPartition;
  };
}

#endif // UNITSELECTORSYSTEM_H
