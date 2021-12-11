#ifndef UNIT_COMPONENTS_H
#define UNIT_COMPONENTS_H

#include <anax/anax.hpp>
#include "../../../core/3rdparty/high_resolution_clock.h"
#include "../../../core/type.h"
#include "../../../core/vector2.h"
#include "../../player.h"
#include "resource_components.h"

namespace strat
{
namespace component
{
  struct UnitTeam : public anax::Component
  {
    explicit UnitTeam(Player::EPlayerNumber team_);

    Player::EPlayerNumber team;
  };

  struct UnitType : public anax::Component
  {
    enum EUnitType : unsigned int
    {
      Worker = 1 << 0,
      Soldier = 1 << 1
    };

    explicit UnitType(EUnitType type_);

    EUnitType type;
  };

  struct UnitState : public anax::Component
  {
    enum class EUnitState : unsigned char
    {
      Idle,
      InitialMovingToAttack,
      InitialMovingToGather,
      InitialMovingToBuild,
      Moving,
      MovingToProximity,
      MovingToAttack,
      MovingToGather,
      MovingToBuild,
      MovingToTarget,
      Attacking,
      GatheringResource,
      ReturningResource,
      Building
    };

    UnitState();

    EUnitState state;
  };

  struct UnitTarget : public anax::Component
  {
    anax::Entity        targetEntity;
    gengine2d::Vector2f targetPosition;
  };

  struct UnitAttributes : public anax::Component
  {
    enum : unsigned int
    {
      UA_ATTRIB_MOVE_SPEED = 0,
      UA_ATTRIB_ARMOR,
      UA_NUMBER_OF_ATTRIBS
    };

    explicit UnitAttributes(std::initializer_list<float> attributes_ = {});

    gengine2d::fixed_vector_t<float, UA_NUMBER_OF_ATTRIBS> attributes;
  };

  // TODO: Change the attack timer and gather timer
  // to a single timer that is action timer
  // and add an action wait timer

  /// Soldier Components
  struct UnitSoldierAttackType : public anax::Component
  {
    enum class EAttackType : unsigned char
    {
      AT_TYPE_MELEE = 0,
      AT_TYPE_PROJECTILE
    };

    explicit UnitSoldierAttackType(EAttackType type_);

    EAttackType type;
  };

  struct UnitSoldierAttackAttributes : public anax::Component
  {
    enum : unsigned int
    {
      UAA_ATTRIB_ATTACK_DAMAGE = 0,
      UAA_ATTRIB_ATTACK_RANGE,
      UAA_ATTRIB_ATTACK_SPEED,
      UAA_ATTRIB_ATTACK_ANIMATION_SPEED,
      UAA_NUMBER_OF_ATTRIBS
    };

    explicit UnitSoldierAttackAttributes(std::initializer_list<float> attributes_ = {});

    gengine2d::fixed_vector_t<float, UAA_NUMBER_OF_ATTRIBS> attributes;
  };

  // Attack timer vs Wait timer:
  // attack timer is the animation time it takes for the unit's attack to hit the target
  // wait timer is the timer after each attack (ie. attack speed)
  struct UnitSoldierAttackTimer : public anax::Component
  {
    mar::high_resolution_stopwatch timer;
  };

  struct UnitSoldierWaitTimer : public anax::Component
  {
    mar::high_resolution_stopwatch timer;
  };

  // TODO: Add recovery timer. Recovery timer is that the unit waits for after each attack before it
  // could move again

  /// Worker Components
  struct UnitWorkerResourceGathering : public anax::Component
  {
    UnitWorkerResourceGathering();

    int resource;
  };

  struct UnitWorkerResourceCollected : public anax::Component
  {
    UnitWorkerResourceCollected();

    gengine2d::fixed_vector_t<int, GlobalResource::GR_NUMBER_OF_RES> collected;
  };

  struct UnitWorkerGatherTimer : public anax::Component
  {
    mar::high_resolution_stopwatch timer;
  };

  struct UnitWorkerCastle : public anax::Component
  {
    explicit UnitWorkerCastle(const anax::Entity& castle_);

    anax::Entity castle;
  };

  struct UnitWorkerBuildingsBuilts : public anax::Component
  {
    explicit UnitWorkerBuildingsBuilts(gengine2d::vector_t<gengine2d::string_t> buildingsBuilt_);

    gengine2d::vector_t<gengine2d::string_t> buildingsBuilt;
  };
}
namespace unit_component_utils
{
  void setStateToIdle(const anax::Entity& entity);
  void setState(const anax::Entity& entity,
                component::UnitState::EUnitState state);
  void setTargetEntity(const anax::Entity& entity,
                       const anax::Entity& targetEntity);
  void setTargetPosition(const anax::Entity& entity,
                         const gengine2d::Vector2f& targetPosition);

  bool sameTeam(const anax::Entity& e1, const anax::Entity& e2);
  bool isWorker(const anax::Entity& entity);
  bool isSoldier(const anax::Entity& entity);
  bool isWorkerCarryingResource(const anax::Entity& entity, int value = 0);
  bool isWorkerCarryingResourceFull(const anax::Entity& entity);

  const component::UnitState::EUnitState& getState(const anax::Entity& entity);
  const float& getAttribute(const anax::Entity& entity, unsigned int attributeId);
  const Player::EPlayerNumber& getTeam(const anax::Entity& entity);
  anax::Entity& getTargetEntity(const anax::Entity& entity);
  const gengine2d::Vector2f& getTargetPosition(const anax::Entity& entity);

  const gengine2d::vector_t<gengine2d::string_t>& getWorkerBuildingsBuilt(const anax::Entity& entity);
  const float& getSoldierAttackAttribute(const anax::Entity& entity, unsigned int attributeId);
}
}

#endif // UNIT_COMPONENTS_H
