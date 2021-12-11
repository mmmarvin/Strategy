#include "building_components.h"

#include "../../utility/component_utility.h"

namespace strat
{
namespace component
{
  BuildingCompleteness::BuildingCompleteness(bool completed_) :
    completed(completed_)
  {
  }

  BuildingType::BuildingType(EBuildingType type_) :
    type(type_)
  {
  }

  BuildingAttackType::BuildingAttackType(EAttackType type_) :
    type(type_)
  {
  }

  BuildingAttackAttributes::BuildingAttackAttributes(std::initializer_list<float> attributes_) :
    attributes(0.f)
  {
    component_utils::copyFitInitListToFixedVec(attributes, attributes_);
  }

  BuildingUnitsTrained::BuildingUnitsTrained(gengine2d::vector_t<gengine2d::string_t> unitsCreated_) :
    unitsTrained(std::move(unitsCreated_))
  {
  }

  BuildingUnitTrainTarget::BuildingUnitTrainTarget() :
    targetPosition(-100.f, -100.f)
  {
  }

  BuildingUnitTrainFlag::BuildingUnitTrainFlag(const anax::Entity& flagEntity_) :
    flagEntity(flagEntity_)
  {
  }
}
namespace building_component_utils
{
  bool isOfType(const anax::Entity& entity, component::BuildingType::EBuildingType type)
  {
    assert(entity.hasComponent<component::BuildingType>());
    return entity.getComponent<component::BuildingType>().type == type;
  }

  const float& getAttackAttributes(const anax::Entity& entity, unsigned int attributeId)
  {
    assert(attributeId < entity.getComponent<component::BuildingAttackAttributes>().attributes.capacity());
    return entity.getComponent<component::BuildingAttackAttributes>().attributes[attributeId];
  }

  const gengine2d::vector_t<gengine2d::string_t>& getUnitsTrained(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BuildingUnitsTrained>());
    return entity.getComponent<component::BuildingUnitsTrained>().unitsTrained;
  }

  const gengine2d::vector_t<std::pair<std::reference_wrapper<const UnitData>, mar::high_resolution_stopwatch>>& getUnitTrainQueue(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BuildingUnitTrainQueue>());
    return entity.getComponent<component::BuildingUnitTrainQueue>().queue;
  }

  anax::Entity& getTrainTargetEntity(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BuildingUnitTrainTarget>());
    return entity.getComponent<component::BuildingUnitTrainTarget>().targetEntity;
  }

  const gengine2d::Vector2f& getTrainTargetPosition(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BuildingUnitTrainTarget>());
    return entity.getComponent<component::BuildingUnitTrainTarget>().targetPosition;
  }

  const anax::Entity& getUnitTrainFlagEntity(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BuildingUnitTrainFlag>());
    return entity.getComponent<component::BuildingUnitTrainFlag>().flagEntity;
  }

  const component::BuildingAttackType::EAttackType& getAttackType(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BuildingAttackType>());
    return entity.getComponent<component::BuildingAttackType>().type;
  }

  void setTrainTargetEntity(const anax::Entity& entity, const anax::Entity& targetEntity)
  {
    assert(entity.hasComponent<component::BuildingUnitTrainTarget>());
    entity.getComponent<component::BuildingUnitTrainTarget>().targetEntity = targetEntity;
  }

  void setTrainTargetPosition(const anax::Entity& entity, const gengine2d::Vector2f& targetPosition)
  {
    assert(entity.hasComponent<component::BuildingUnitTrainTarget>());
    entity.getComponent<component::BuildingUnitTrainTarget>().targetPosition = targetPosition;
  }

  void addToUnitTrainQueue(const anax::Entity& entity,
                           const std::pair<std::reference_wrapper<const UnitData>,
                                           mar::high_resolution_stopwatch>& trainQueuePair)
  {
    assert(entity.hasComponent<component::BuildingUnitTrainQueue>());
    entity.getComponent<component::BuildingUnitTrainQueue>().queue.push_back(trainQueuePair);
  }

  void resetTrainTarget(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BuildingUnitTrainTarget>());
    auto& buildingTrainUnitTargetC = entity.getComponent<component::BuildingUnitTrainTarget>();
    buildingTrainUnitTargetC.targetEntity = anax::Entity();
    buildingTrainUnitTargetC.targetPosition = gengine2d::Vector2f(0.f, 0.f);
  }
}
}
