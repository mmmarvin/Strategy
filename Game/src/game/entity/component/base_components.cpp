#include "base_components.h"

#include "../../utility/component_utility.h"
#include "../../game_def.h"

namespace strat
{
namespace component
{
  BaseName::BaseName(gengine2d::string_t name_) :
    name(std::move(name_))
  {
  }

  BasePosition::BasePosition(const gengine2d::Vector2f& position_) :
    oldPosition(position_),
    position(position_)
  {
  }

  BaseSpatialPosition::BaseSpatialPosition(detail::SpatialParitionAnaxObjectHandle spatialPosition_) :
    spatialPosition(spatialPosition_)
  {
  }

  BaseSprite::BaseSprite()
  {
    // TODO: Remove this?
    sprite.setPosition(0, 0);
    sprite.setSize(gengine2d::Vector2f(TILE_SIZE / 2.f, TILE_SIZE / 2.f));
    sprite.setOrigin(sprite.getSize().x / 2.f, sprite.getSize().y);
  }

  BaseCollider::BaseCollider(float x, float y, float radius) :
    collider(x, y, radius)
  {
  }

  BaseCollider::BaseCollider(const gengine2d::Circlef& collider_) :
    collider(collider_)
  {
  }

  BaseEntityType::BaseEntityType(EEntityType type_) :
    type(type_)
  {
  }

  BaseAttributes::BaseAttributes(std::initializer_list<float> attributes_) :
    attributes(0.f)
  {
    assert(attributes_.size() == attributes.capacity());
    component_utils::copyFitInitListToFixedVec(attributes, attributes_);
  }

  BaseFlags::BaseFlags(std::initializer_list<bool> flags_)
  {
    assert(flags_.size() == flags.size());
    for(auto& flag : flags) {
      flag = false;
    }

    component_utils::copyFitInitListToArray(flags, flags_);
  }
}
namespace base_component_utils
{
  bool isValidOrReset(anax::Entity& entity)
  {
    if(entity.isValid()) {
      return true;
    }

    //if entity is an empty entity, we don't reset
    // just return false
    if(entity == anax::Entity()) {
      return false;
    }

    // else, reset the entity to an empty entity,
    // and return false
    entity = anax::Entity();
    return false;
  }

  bool isOfType(const anax::Entity& entity, component::BaseEntityType::EEntityType type)
  {
    return entity.getComponent<component::BaseEntityType>().type == type;
  }

  const gengine2d::Vector2f& getPosition(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BasePosition>());
    return entity.getComponent<component::BasePosition>().position;
  }

  const gengine2d::Vector2f& getLastPosition(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BasePosition>());
    return entity.getComponent<component::BasePosition>().oldPosition;
  }

  const gengine2d::Circlef& getBaseCollider(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BaseCollider>());
    return entity.getComponent<component::BaseCollider>().collider;
  }

  detail::SpatialParitionAnaxObjectHandle& getSpatialPositionHandle(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BaseSpatialPosition>());
    return entity.getComponent<component::BaseSpatialPosition>().spatialPosition;
  }

  const gengine2d::string_t& getName(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::BaseName>());
    return entity.getComponent<component::BaseName>().name;
  }

  const float& getAttribute(const anax::Entity& entity, unsigned int attributeId)
  {
    assert(attributeId < entity.getComponent<component::BaseAttributes>().attributes.capacity());
    return entity.getComponent<component::BaseAttributes>().attributes[attributeId];
  }

  const bool& getFlag(const anax::Entity& entity, unsigned int flagId)
  {
    assert(flagId < entity.getComponent<component::BaseFlags>().flags.size());
    return entity.getComponent<component::BaseFlags>().flags[flagId];
  }

  void setAllPosition(const anax::Entity& entity, const gengine2d::Vector2f& position)
  {
    setPosition(entity, position);
    setLastPosition(entity, position);
  }

  void setPosition(const anax::Entity& entity, const gengine2d::Vector2f& position)
  {
    assert(entity.hasComponent<component::BasePosition>());
    entity.getComponent<component::BasePosition>().position = position;
  }

  void setLastPosition(const anax::Entity& entity, const gengine2d::Vector2f& position)
  {
    assert(entity.hasComponent<component::BasePosition>());
    entity.getComponent<component::BasePosition>().oldPosition = position;
  }
}
}
