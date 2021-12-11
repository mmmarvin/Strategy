#include "projectile_components.h"

#include "../../utility/component_utility.h"
#include "base_components.h"

namespace strat
{
namespace component
{
  ProjectileOwner::ProjectileOwner(const anax::Entity& owner_) :
    owner(owner_)
  {
  }

  ProjectileAttributes::ProjectileAttributes(std::initializer_list<float> attributes_) :
    attributes(0.f)
  {
    assert(attributes_.size() == attributes.capacity());
    component_utils::copyFitInitListToFixedVec(attributes, attributes_);
  }

  ProjectileTargetEntity::ProjectileTargetEntity(const anax::Entity& targetEntity_) :
    targetEntity(targetEntity_)
  {
    if(targetEntity.isValid()) {
      targetPosition = targetEntity.getComponent<component::BasePosition>().position;
    }
  }

  ProjectileTargetPosition::ProjectileTargetPosition(const gengine2d::Vector2f& targetPosition_) :
    targetPosition(targetPosition_)
  {
  }
}
namespace projectile_component_utils
{
  void setProjectileTargetEntity(const anax::Entity& entity,
                                 const anax::Entity& targetEntity)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Projectile);

    auto& projectileTargetEntityC = entity.getComponent<component::ProjectileTargetEntity>();
    projectileTargetEntityC.targetEntity = targetEntity;
    if(targetEntity.isValid()) {
      projectileTargetEntityC.targetPosition = targetEntity.getComponent<component::BasePosition>().position;
    }
  }

  void setProjectileTargetPosition(const anax::Entity& entity,
                                   const gengine2d::Vector2f& targetPosition)
  {
    assert(entity.getComponent<component::BaseEntityType>().type ==
           component::BaseEntityType::EEntityType::Projectile);

    entity.getComponent<component::ProjectileTargetPosition>().targetPosition = targetPosition;
  }
}
}
