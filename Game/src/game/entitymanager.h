#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <unordered_set>
#include <anax/anax.hpp>
#include "data/base_data.h"
#include "data/building_data.h"
#include "data/unit_data.h"
#include "entity/component/base_components.h"
#include "entity/component/unit_components.h"
#include "fogmanager.h"
#include "global_resource.h"
#include "spatialpartition.h"

namespace strat
{
  class Map;
  class EntityManager
  {
  public:
    EntityManager(const Map& map,
                  FogManager& fogs);

    anax::Entity createUnit(const UnitData& unitData,
                            Player::EPlayerNumber team,
                            gengine2d::Vector2f position,
                            const anax::Entity& castle);
    anax::Entity createBuilding(const BuildingData& buildingData,
                                Player::EPlayerNumber team,
                                gengine2d::Vector2f position,
                                bool completed);
    anax::Entity createResource(gengine2d::Vector2f position,
                                GlobalResource::EGlobalResouceType type);

    anax::Entity createProjectile(const anax::Entity& owner,
                                  Player::EPlayerNumber team,
                                  gengine2d::Vector2f position,
                                  const gengine2d::Vector2f& targetPosition);
    anax::Entity createHomingProjectile(const anax::Entity& owner,
                                        Player::EPlayerNumber team,
                                        gengine2d::Vector2f position,
                                        const anax::Entity& targetEntity);

    anax::Entity createFlag(const anax::Entity& owner,
                            Player::EPlayerNumber team,
                            gengine2d::Vector2f position);
    anax::Entity createFlagTimed(gengine2d::Vector2f position);

    void scheduleForDeletion(const anax::Entity& entity);

    spatial_partition_t& getSpatialPartition() { return m_spatialPartition; }
    const spatial_partition_t& getSpatialPartition() const { return m_spatialPartition; }

  private:
    anax::Entity createBaseEntity(const BaseData& baseData,
                                  gengine2d::Vector2f& position,
                                  component::BaseEntityType::EEntityType type,
                                  bool moveToOpenSpot = false);
    anax::Entity createBaseNonSpatialEntity(const BaseData& baseData,
                                            gengine2d::Vector2f& position,
                                            component::BaseEntityType::EEntityType type,
                                            bool moveToOpenSpot = false);
    anax::Entity createFlagBase(gengine2d::Vector2f& position);

    void addUnitComponents(const UnitData& unitData,
                           anax::Entity& entity,
                           Player::EPlayerNumber team,
                           component::UnitType::EUnitType type);

    void refresh();

    friend class GameWorld;

    gengine2d::vector_t<anax::Entity>               m_entitiesDeleted;
    std::unordered_set<anax::Entity::Id::int_type>  m_entitiesDeletedId;

    anax::World                                     m_entityWorld;
    spatial_partition_t                             m_spatialPartition;
    const Map*                                      m_map;
    FogManager*                                     m_fogs;
  };
}

#endif // ENTITYMANAGER_H
