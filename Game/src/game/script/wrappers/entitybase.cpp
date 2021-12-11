#include "entitybase.h"

#include "../../entity/component/base_components.h"

namespace strat
{
namespace lua_wrappers
{
  EntityBase::EntityBase(const anax::Entity& entity) :
    m_entity(entity)
  {
  }

  void EntityBase::setPosition(const gengine2d::Vector2f& position)
  {
    base_component_utils::setAllPosition(m_entity, position);
  }

  const gengine2d::Vector2f& EntityBase::getPosition() const
  {
    return base_component_utils::getPosition(m_entity);
  }
}
}
