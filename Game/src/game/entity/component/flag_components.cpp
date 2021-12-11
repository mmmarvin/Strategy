#include "flag_components.h"

#include <cassert>
#include "base_components.h"

namespace strat
{
namespace component
{
  FlagOwner::FlagOwner(const anax::Entity& owner_) :
    owner(owner_)
  {
  }
}
namespace flag_component_utils
{
  const anax::Entity& getOwner(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::FlagOwner>());
    return entity.getComponent<component::FlagOwner>().owner;
  }

  const mar::high_resolution_stopwatch& getTimer(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::FlagTimer>());
    return entity.getComponent<component::FlagTimer>().timer;
  }

  void resetTimer(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::FlagTimer>());
    entity.getComponent<component::FlagTimer>().timer.restart();
  }

  void stopTimer(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::FlagTimer>());
    entity.getComponent<component::FlagTimer>().timer.stop();
  }

  void showTimedFlag(const anax::Entity& entity, const gengine2d::Vector2f& position)
  {
    base_component_utils::setAllPosition(entity, position);
    resetTimer(entity);
  }
}
}
