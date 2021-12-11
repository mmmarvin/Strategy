#ifndef UNITSYSTEM_H
#define UNITSYSTEM_H

#include <anax/anax.hpp>
#include "../../../core/3rdparty/high_resolution_clock.h"
#include "../../spatialpartition.h"
#include "../component/base_components.h"
#include "../component/building_components.h"
#include "../component/unit_components.h"
#include "attackbasesystem.h"

namespace strat
{
  class EntityManager;
  class GameWorld;
  class Map;
namespace system
{
  class UnitSystem : public anax::System<anax::Requires<component::BasePosition,
                                                        component::BaseSpatialPosition,
                                                        component::BaseCollider,
                                                        component::BaseFlags,
                                                        component::UnitState,
                                                        component::UnitType,
                                                        component::UnitTeam,
                                                        component::UnitTarget,
                                                        component::UnitAttributes>>,
                     private AttackBaseSystem
  {
    class CastleFinderSystem : public anax::System<anax::Requires<component::BasePosition,
                                                                  component::BuildingType>>
    {
    public:
      CastleFinderSystem() {}

      anax::Entity findClosestCastle(const anax::Entity& entity) const;
    };

  public:
    explicit UnitSystem(EntityManager& entityManager,
                        GlobalResource& globalResource,
                        const Map& map,
                        FogManager& fogs);

    void update(float dt);

  private:
    friend class ::strat::GameWorld;

    CastleFinderSystem& getCastleFinderSystem() { return m_castleFinderSystem; }

    void initialMoveToAttack(const anax::Entity& entity);
    void initialMoveToGather(const anax::Entity& entity);
    void initialMoveToBuild(const anax::Entity& entity);

    void tryMove(const anax::Entity& entity, float dt, float stopDistance);

    void tryMoveToAttack(const anax::Entity& entity, float dt);
    void tryMoveToGather(const anax::Entity& entity, float dt);
    void tryMoveToBuild(const anax::Entity& entity, float dt);
    void tryMoveToTarget(const anax::Entity& entity, float dt);

    void tryAttack(const anax::Entity& entity);
    void tryGather(const anax::Entity& entity);
    void tryReturnResource(const anax::Entity& entity, float dt);
    void tryBuild(const anax::Entity& entity);

    void performMove(const anax::Entity& entity,
                     const gengine2d::Vector2f& targetPosition,
                     float dt);
    void performMoveToStopDistance(const anax::Entity& entity,
                                   const gengine2d::Vector2f& targetPosition,
                                   float stopDistance,
                                   float dt);
    void performAttack(const anax::Entity& entity);
    void performReturnResource(const anax::Entity& entity);

    void performMoveToTarget(const anax::Entity& entity,
                         const anax::Entity& target,
                         float stopDistance,
                         component::UnitState::EUnitState finalState,
                         void (UnitSystem::*rangeReachedEvent)(const anax::Entity&),
                         float dt);

    void findClosestResourceToGatherResource(const anax::Entity& entity);

    CastleFinderSystem          m_castleFinderSystem;
    mar::high_resolution_clock  m_enemySearchTimer;
    GlobalResource*             m_globalResource;
    const Map*                  m_map;
  };
}
}

#endif // UNITSYSTEM_H
