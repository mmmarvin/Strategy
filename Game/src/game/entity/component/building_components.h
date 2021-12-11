#ifndef BUILDING_COMPONENTS_H
#define BUILDING_COMPONENTS_H

#include <anax/anax.hpp>
#include "../../../core/3rdparty/high_resolution_clock.h"
#include "../../../core/type.h"
#include "../../../core/vector2.h"
#include "unit_components.h"

namespace strat
{
  class UnitData;
namespace component
{
  struct BuildingCompleteness : public anax::Component
  {
    explicit BuildingCompleteness(bool completed_);

    bool completed;
  };

  struct BuildingType : public anax::Component
  {
    enum class EBuildingType : unsigned char
    {
      BT_TYPE_CASTLE = 0,
      BT_TYPE_TRAINER,
      BT_TYPE_TOWER,
      BT_TYPE_WALL,
      BT_TYPE_OTHER
    };

    explicit BuildingType(EBuildingType type_);

    EBuildingType type;
  };

  struct BuildingAttackType : public anax::Component
  {
    enum class EAttackType : unsigned int
    {
      AT_TYPE_NONE = 0,
      AT_TYPE_MELEE,
      AT_TYPE_PROJECTILE
    };

    explicit BuildingAttackType(EAttackType type_);

    EAttackType type;
  };

  struct BuildingAttackAttributes : public anax::Component
  {
    enum : unsigned int
    {
      BUA_ATTRIB_ATTACK_DAMAGE = 0,
      BUA_ATTRIB_ATTACK_RANGE,
      BUA_ATTRIB_ATTACK_SPEED,
      BUA_NUMBER_OF_ATTRIBS
    };

    explicit BuildingAttackAttributes(std::initializer_list<float> attributes_);

    gengine2d::fixed_vector_t<float, BUA_NUMBER_OF_ATTRIBS> attributes;
  };

  struct BuildingAttackTimer : public anax::Component
  {
    mar::high_resolution_clock timer;
  };

  struct BuildingUnitsTrained : public anax::Component
  {
    explicit BuildingUnitsTrained(gengine2d::vector_t<gengine2d::string_t> unitsCreated_);

    gengine2d::vector_t<gengine2d::string_t> unitsTrained;
  };

  struct BuildingUnitTrainQueue : public anax::Component
  {
    gengine2d::vector_t<std::pair<std::reference_wrapper<const UnitData>, mar::high_resolution_stopwatch>> queue;
  };

  struct BuildingUnitTrainTarget : public anax::Component
  {
    BuildingUnitTrainTarget();

    anax::Entity        targetEntity;
    gengine2d::Vector2f targetPosition;
  };

  struct BuildingUnitTrainFlag : public anax::Component
  {
    explicit BuildingUnitTrainFlag(const anax::Entity& flagEntity_);

    anax::Entity  flagEntity;
  };
}
namespace building_component_utils
{
  bool isOfType(const anax::Entity& entity, component::BuildingType::EBuildingType type);

  const float& getAttackAttributes(const anax::Entity& entity, unsigned int attributeId);
  const gengine2d::vector_t<gengine2d::string_t>& getUnitsTrained(const anax::Entity& entity);
  const gengine2d::vector_t<std::pair<std::reference_wrapper<const UnitData>, mar::high_resolution_stopwatch>>& getUnitTrainQueue(const anax::Entity& entity);
  anax::Entity& getTrainTargetEntity(const anax::Entity& entity);
  const gengine2d::Vector2f& getTrainTargetPosition(const anax::Entity& entity);
  const anax::Entity& getUnitTrainFlagEntity(const anax::Entity& entity);
  const component::BuildingAttackType::EAttackType& getAttackType(const anax::Entity& entity);

  void setTrainTargetEntity(const anax::Entity& entity, const anax::Entity& targetEntity);
  void setTrainTargetPosition(const anax::Entity& entity, const gengine2d::Vector2f& targetPosition);
  void addToUnitTrainQueue(const anax::Entity& entity,
                           const std::pair<std::reference_wrapper<const UnitData>,
                                           mar::high_resolution_stopwatch>& trainQueuePair);
  void resetTrainTarget(const anax::Entity& entity);
}
}

#endif // BUILDING_COMPONENTS_H
