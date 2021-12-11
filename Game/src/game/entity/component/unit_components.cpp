#include "unit_components.h"

#include "../../utility/component_utility.h"
#include "base_components.h"

namespace strat
{
namespace component
{
  UnitTeam::UnitTeam(Player::EPlayerNumber team_) :
    team(team_)
  {
  }

  UnitType::UnitType(EUnitType type_) :
    type(type_)
  {
  }

  UnitState::UnitState() :
    state(EUnitState::Idle)
  {
  }

  UnitAttributes::UnitAttributes(std::initializer_list<float> attributes_) :
     attributes(0.f)
  {
    assert(attributes_.size() == attributes.capacity());
    component_utils::copyFitInitListToFixedVec(attributes, attributes_);
  }

  UnitSoldierAttackType::UnitSoldierAttackType(EAttackType type_) :
    type(type_)
  {
  }

  UnitSoldierAttackAttributes::UnitSoldierAttackAttributes(std::initializer_list<float> attributes_) :
    attributes(0.f)
  {
    assert(attributes_.size() == attributes.capacity());
    component_utils::copyFitInitListToFixedVec(attributes, attributes_);
  }

  UnitWorkerResourceGathering::UnitWorkerResourceGathering() :
    resource(-1)
  {
  }

  UnitWorkerResourceCollected::UnitWorkerResourceCollected() :
    collected(0)
  {
  }

  UnitWorkerCastle::UnitWorkerCastle(const anax::Entity& castle_) :
    castle(castle_)
  {
  }

  UnitWorkerBuildingsBuilts::UnitWorkerBuildingsBuilts(gengine2d::vector_t<gengine2d::string_t> buildingsBuilt_) :
    buildingsBuilt(std::move(buildingsBuilt_))
  {
  }
}
namespace unit_component_utils
{
  void setStateToIdle(const anax::Entity& entity)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit);

    entity.getComponent<component::UnitState>().state = component::UnitState::EUnitState::Idle;

    auto& basePositionC = entity.getComponent<component::BasePosition>();
    basePositionC.oldPosition = basePositionC.position;
  }

  void setState(const anax::Entity& entity,
                component::UnitState::EUnitState state)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit);

    entity.getComponent<component::UnitState>().state =  state;
  }

  void setTargetEntity(const anax::Entity& entity,
                       const anax::Entity& targetEntity)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit ||
           entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Building);

    entity.getComponent<component::UnitTarget>().targetEntity = targetEntity;
  }

  void setTargetPosition(const anax::Entity& entity,
                         const gengine2d::Vector2f& targetPosition)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit ||
           entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Building);

    entity.getComponent<component::UnitTarget>().targetPosition = targetPosition;
  }

  bool sameTeam(const anax::Entity& e1, const anax::Entity& e2)
  {
    assert(e1.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit ||
           e1.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Building ||
           e1.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Projectile);
    assert(e2.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit ||
           e2.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Building ||
           e2.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Projectile);

    return e1.getComponent<component::UnitTeam>().team == e2.getComponent<component::UnitTeam>().team;
  }

  bool isWorker(const anax::Entity& entity)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit);
    assert(entity.hasComponent<component::UnitType>());

    return entity.getComponent<component::UnitType>().type & component::UnitType::Worker;
  }

  bool isSoldier(const anax::Entity& entity)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit);
    assert(entity.hasComponent<component::UnitType>());

    return entity.getComponent<component::UnitType>().type & component::UnitType::Soldier;
  }

  bool isWorkerCarryingResource(const anax::Entity& entity, int value)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit &&
           isWorker(entity));

    const auto& resourceCollected = entity.getComponent<component::UnitWorkerResourceCollected>().collected;
    for(auto resource : resourceCollected) {
      if(resource >= value) {
        return true;
      }
    }

    return false;
  }

  bool isWorkerCarryingResourceFull(const anax::Entity& entity)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit &&
           isWorker(entity));

    // TODO: Change this value to depend on each unit
    return isWorkerCarryingResource(entity, 10);
  }

  const component::UnitState::EUnitState& getState(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::UnitState>());
    return entity.getComponent<component::UnitState>().state;
  }

  const float& getAttribute(const anax::Entity& entity, unsigned int attributeId)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit);
    assert(attributeId < entity.getComponent<component::UnitAttributes>().attributes.capacity());

    return entity.getComponent<component::UnitAttributes>().attributes[attributeId];
  }

  const Player::EPlayerNumber& getTeam(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::UnitTeam>());
    return entity.getComponent<component::UnitTeam>().team;
  }

  anax::Entity& getTargetEntity(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::UnitTarget>());
    return entity.getComponent<component::UnitTarget>().targetEntity;
  }

  const gengine2d::Vector2f& getTargetPosition(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::UnitTarget>());
    return entity.getComponent<component::UnitTarget>().targetPosition;
  }

  const gengine2d::vector_t<gengine2d::string_t>& getWorkerBuildingsBuilt(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::UnitWorkerBuildingsBuilts>());
    return entity.getComponent<component::UnitWorkerBuildingsBuilts>().buildingsBuilt;
  }

  const float& getSoldierAttackAttribute(const anax::Entity& entity, unsigned int attributeId)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Unit);
    assert(attributeId <
           entity.getComponent<component::UnitSoldierAttackAttributes>().attributes.capacity());

    return entity.getComponent<component::UnitSoldierAttackAttributes>().attributes[attributeId];
  }
}
}
