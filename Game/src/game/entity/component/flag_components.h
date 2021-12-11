#ifndef FLAG_COMPONENTS_H
#define FLAG_COMPONENTS_H

#include <anax/anax.hpp>
#include "../../../core/3rdparty/high_resolution_clock.h"
#include "../../../core/vector2.h"

namespace strat
{
namespace component
{
  struct FlagOwner : public anax::Component
  {
    explicit FlagOwner(const anax::Entity& owner_);

    anax::Entity owner;
  };

  struct FlagTimer : public anax::Component
  {
    mar::high_resolution_stopwatch timer;
  };
}
namespace flag_component_utils
{
  const anax::Entity& getOwner(const anax::Entity& entity);
  const mar::high_resolution_stopwatch& getTimer(const anax::Entity& entity);

  void resetTimer(const anax::Entity& entity);
  void stopTimer(const anax::Entity& entity);

  void showTimedFlag(const anax::Entity& entity, const gengine2d::Vector2f& position);
}
}

#endif // FLAG_COMPONENTS_H
