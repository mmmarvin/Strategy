#ifndef BASE_COMPONENTS_H
#define BASE_COMPONENTS_H

#include <SFML/Graphics.hpp>
#include <anax/anax.hpp>
#include "../../../core/collider/circle.h"
#include "../../spatialpartition.h"

namespace strat
{
namespace component
{
  struct BaseName : public anax::Component
  {
    explicit BaseName(gengine2d::string_t name_);

    gengine2d::string_t name;
  };

  struct BasePosition : public anax::Component
  {
    explicit BasePosition(const gengine2d::Vector2f& position_);

    gengine2d::Vector2f oldPosition;
    gengine2d::Vector2f position;
  };

  struct BaseSpatialPosition : public anax::Component
  {
    explicit BaseSpatialPosition(detail::SpatialParitionAnaxObjectHandle spatialPosition_);

    detail::SpatialParitionAnaxObjectHandle spatialPosition;
  };

  struct BaseSprite : public anax::Component
  {
    BaseSprite();

    sf::RectangleShape sprite;
  };

  struct BaseCollider : public anax::Component
  {
    BaseCollider(float x, float y, float radius);
    explicit BaseCollider(const gengine2d::Circlef& collider_);

    gengine2d::Circlef collider;
  };

  struct BaseEntityType : public anax::Component
  {
    enum class EEntityType : unsigned char
    {
      Unit,
      Building,
      Resource,
      Props,
      Projectile,
      Flag
    };

    explicit BaseEntityType(EEntityType type_);

    EEntityType type;
  };

  struct BaseAttributes : public anax::Component
  {
    enum : unsigned int
    {
      BA_ATTRIB_HP = 0,
      BA_ATTRIB_MAX_HP,
      BA_NUMBER_OF_ATTRIBS
    };

    explicit BaseAttributes(std::initializer_list<float> attributes_ = {});

    gengine2d::fixed_vector_t<float, BA_NUMBER_OF_ATTRIBS> attributes;
  };

  // TODO: Change this bitsets?
  struct BaseFlags : public anax::Component
  {
    enum : unsigned int
    {
      BF_FLAG_IS_SELECTED = 0,
      BF_FLAG_IS_COLLIDEABLE,
      BF_FLAG_IS_INVULNERABLE,
      BF_NUMBER_OF_FLAGS
    };

    explicit BaseFlags(std::initializer_list<bool> flags_ = {});

    std::array<bool, BF_NUMBER_OF_FLAGS> flags;
  };
}
namespace base_component_utils
{
  bool isValidOrReset(anax::Entity& entity);
  bool isOfType(const anax::Entity& entity, component::BaseEntityType::EEntityType type);

  const gengine2d::Vector2f& getPosition(const anax::Entity& entity);
  const gengine2d::Vector2f& getLastPosition(const anax::Entity& entity);
  const gengine2d::Circlef& getBaseCollider(const anax::Entity& entity);
  detail::SpatialParitionAnaxObjectHandle& getSpatialPositionHandle(const anax::Entity& entity);

  const gengine2d::string_t& getName(const anax::Entity& entity);
  const float& getAttribute(const anax::Entity& entity, unsigned int attributeId);
  const bool& getFlag(const anax::Entity& entity, unsigned int flagId);

  void setAllPosition(const anax::Entity& entity, const gengine2d::Vector2f& position);
  void setPosition(const anax::Entity& entity, const gengine2d::Vector2f& position);
  void setLastPosition(const anax::Entity& entity, const gengine2d::Vector2f& position);
}
}

#endif // BASE_COMPONENTS_H
