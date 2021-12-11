#ifndef PROJECTILE_COMPONENTS_H
#define PROJECTILE_COMPONENTS_H

#include <anax/anax.hpp>
#include "../../../core/collider/circle.h"
#include "../../../core/type.h"
#include "../../../core/vector2.h"

namespace strat
{
namespace component
{
  struct ProjectileOwner : public anax::Component
  {
    explicit ProjectileOwner(const anax::Entity& owner_);

    anax::Entity owner;
  };

  struct ProjectileAttributes : public anax::Component
  {
    enum : unsigned int
    {
      PA_ATTRIB_MOVE_SPEED = 0,
      PA_ATTRIB_ATTACK_DAMAGE,
      PA_ATTRIB_ATTACK_DAMAGE_DROPOFF,
      PA_ATTRIB_ATTACK_DAMAGE_MIN,
      PA_ATTRIB_DAMAGE_RADIUS,
      PA_NUMBER_OF_ATTRIBS
    };

    explicit ProjectileAttributes(std::initializer_list<float> attributes_ = {});

    gengine2d::fixed_vector_t<float, PA_NUMBER_OF_ATTRIBS> attributes;
  };

  struct ProjectileHitCollider : public anax::Component
  {
    ProjectileHitCollider(float x, float y, float radius);

    gengine2d::Circlef collider;
  };

  struct ProjectileTargetEntity : public anax::Component
  {
    explicit ProjectileTargetEntity(const anax::Entity& targetEntity_);

    anax::Entity        targetEntity;
    gengine2d::Vector2f targetPosition;
  };

  struct ProjectileTargetPosition : public anax::Component
  {
    explicit ProjectileTargetPosition(const gengine2d::Vector2f& targetPosition_);

    gengine2d::Vector2f targetPosition;
  };
}
namespace projectile_component_utils
{
  void setProjectileTargetEntity(const anax::Entity& entity,
                                 const anax::Entity& targetEntity);
  void setProjectileTargetPosition(const anax::Entity& entity,
                                   const gengine2d::Vector2f& targetPosition);
}
}

#endif // PROJECTILE_COMPONENTS_H
