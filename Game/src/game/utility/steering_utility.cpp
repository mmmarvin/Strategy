#include "steering_utility.h"

#include "../entity/component/base_components.h"
#include "../game_def.h"

namespace strat
{
namespace steering_utils
{
  bool accumulateForce(gengine2d::Vector2f& total,
                       const gengine2d::Vector2f& force,
                       float moveSpeed)
  {
    // Code from Mat Buckland's "Programming Game AI by Example" book source code
    // from http://www.ai-junkies.com/
    auto mag = gengine2d::magnitude(total);
    auto magRem = moveSpeed - mag;
    if(magRem <= 0.f)
      return false;

    auto magToAdd = gengine2d::magnitude(force);
    if(magToAdd < magRem)
      total += force;
    else
      total += (gengine2d::normalize(force) * moveSpeed);

    return true;
  }

  gengine2d::Vector2f seek(const gengine2d::Vector2f& position,
                           const gengine2d::Vector2f& target,
                           const gengine2d::Vector2f& currentVelocity,
                           float moveSpeed)
  {
    auto desiredVelocity = gengine2d::normalize(target - position) * moveSpeed;
    return desiredVelocity - currentVelocity;
  }

  gengine2d::Vector2f separate(const anax::Entity& entity,
                               const gengine2d::Vector2f& position,
                               gengine2d::Vector2f& currentVelocity,
                               float moveSpeed,
                               const spatial_partition_t& spatialPartition)
  {
    // Code is based on Daniel Shiffman's "Nature of Code"
    // https://github.com/nature-of-code
    // http://natureofcode.com
    gengine2d::Vector2f force;
    float count = 0.f;

    if(entity.getComponent<component::BaseFlags>().flags[component::BaseFlags::BF_FLAG_IS_COLLIDEABLE]) {
      const auto& collider = entity.getComponent<component::BaseCollider>().collider;
      auto colliderPosition = position - collider.center;

      auto [spx, spy] = position;
      spx /= TILE_SIZE;
      spy /= TILE_SIZE;
      spatialPartition.forEach(spx - 1.f, spy - 1.f, spx + 1.f, spy + 1.f,
      [&position, &entity, &force, &count, &collider, &colliderPosition]
      (const anax::Entity& otherEntity)
      {
        // check for self, and target entity (if it is target entity we don't want to
        // flee away from it
        if(entity != otherEntity &&
           !base_component_utils::isOfType(otherEntity,
                                           component::BaseEntityType::EEntityType::Projectile) &&
           !base_component_utils::isOfType(otherEntity,
                                           component::BaseEntityType::EEntityType::Flag) &&
           otherEntity.getComponent<component::BaseFlags>().flags[component::BaseFlags::BF_FLAG_IS_COLLIDEABLE]) {
            const auto& otherPosition = otherEntity.template getComponent<component::BasePosition>().position;

            if(otherEntity.template hasComponent<component::BaseCollider>()) {
              const auto& otherCollider = otherEntity.template getComponent<component::BaseCollider>().collider;
              auto otherColliderPosition = otherPosition - otherCollider.center;

              auto d = gengine2d::distance(colliderPosition, otherColliderPosition);
              if(d < (collider.radius + otherCollider.radius + UNIT_SIZE)) {
                auto sf = gengine2d::normalize(colliderPosition - otherColliderPosition);
                sf /= d;
                force += sf;
                count += 1.f;
              }
            }
        }
      });

      if(count > 0.00001f) {
        force /= count;
        force = gengine2d::normalize(force);
        force *= (moveSpeed * 2.f);
        return force - currentVelocity;
      }
    }

    return gengine2d::Vector2f(0.f, 0.f);
  }

  gengine2d::Vector2f flee(const gengine2d::Vector2f& position,
                           const gengine2d::Vector2f& target,
                           const gengine2d::Vector2f& currentVelocity,
                           float moveSpeed)
  {
    // Code from Mat Buckland's "Programming Game AI by Example" book source code
    // from http://www.ai-junkies.com/
    auto desiredVelocity = gengine2d::normalize(position - target) * moveSpeed;
    return desiredVelocity - currentVelocity;
  }
}
}
