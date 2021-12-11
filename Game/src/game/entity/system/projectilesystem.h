#ifndef PROJECTILESYSTEM_H
#define PROJECTILESYSTEM_H

#include <anax/anax.hpp>
#include "../component/base_components.h"
#include "../component/projectile_components.h"
#include "../component/unit_components.h"
#include "attackbasesystem.h"

namespace strat
{
  class EntityManager;
namespace system
{
  class ProjectileSystem : public anax::System<anax::Requires<component::BasePosition,
                                                              component::UnitTeam,
                                                              component::ProjectileAttributes,
                                                              component::ProjectileTargetPosition>>,
                           private AttackBaseSystem
  {
  public:
    explicit ProjectileSystem(EntityManager& entityManager,
                              const Map& map,
                              FogManager& fogs);

    void draw() const;
    void update(float dt);

  private:
//    using circle_shape_type = sf_ext::IsometricCircleShape;

//    gengine2d::vector_t<std::pair<circle_shape_type, mar::high_resolution_clock>> m_blastRadiuses;
  };

  class ProjectileHomingSystem : public anax::System<anax::Requires<component::BasePosition,
                                                                    component::UnitTeam,
                                                                    component::ProjectileAttributes,
                                                                    component::ProjectileTargetEntity>>,
                                 private AttackBaseSystem
  {
  public:
    explicit ProjectileHomingSystem(EntityManager& entityManager,
                                    const Map& map,
                                    FogManager& fogs);

    void draw() const;
    void update(float dt);

  private:
//    using circle_shape_type = sf_ext::IsometricCircleShape;

//    gengine2d::vector_t<std::pair<circle_shape_type, mar::high_resolution_clock>> m_blastRadiuses;
  };
}
}

#endif // PROJECTILESYSTEM_H
