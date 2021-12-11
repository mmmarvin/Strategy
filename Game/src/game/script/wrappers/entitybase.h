#ifndef ENTITYBASE_H
#define ENTITYBASE_H

#include <anax/anax.hpp>
#include "../../../core/type.h"
#include "../../../core/vector2.h"

namespace strat
{
namespace lua_wrappers
{
  class EntityBase
  {
  public:
    explicit EntityBase(const anax::Entity& entity);

    void setPosition(const gengine2d::Vector2f& position);
    const gengine2d::Vector2f& getPosition() const;

  private:
    anax::Entity m_entity;
  };
}
}

#endif // ENTITYBASE_H
