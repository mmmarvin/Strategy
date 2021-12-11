#include "debug_common.h"

namespace strat
{
namespace string_converter_utils
{
  gengine2d::string_t to_string(component::UnitState::EUnitState state)
  {
    switch(state) {
    case component::UnitState::EUnitState::Idle:
      return "Idle";
    case component::UnitState::EUnitState::InitialMovingToAttack:
      return "IMoving To Attack";
    case component::UnitState::EUnitState::InitialMovingToBuild:
      return "IMoving To Build";
    case component::UnitState::EUnitState::InitialMovingToGather:
      return "IMoving To Gather";
    case component::UnitState::EUnitState::Moving:
      return "Moving";
    case component::UnitState::EUnitState::MovingToProximity:
      return "Moving";
    case component::UnitState::EUnitState::MovingToAttack:
      return "Moving To Attack";
    case component::UnitState::EUnitState::MovingToBuild:
      return "Moving To Build";
    case component::UnitState::EUnitState::MovingToGather:
      return "Moving To Gather";
    case component::UnitState::EUnitState::MovingToTarget:
      return "Moving To Target";
    case component::UnitState::EUnitState::Attacking:
      return "Attacking";
    case component::UnitState::EUnitState::GatheringResource:
      return "Gathering Resource";
    case component::UnitState::EUnitState::ReturningResource:
      return "Returning To Base";
    case component::UnitState::EUnitState::Building:
      return "Building";
    default:
        break;
    }

    return "";
  }
}
}
