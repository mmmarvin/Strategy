#include "resource_components.h"

#include "../../utility/component_utility.h"

namespace strat
{
namespace component
{
  ResourceType::ResourceType(GlobalResource::EGlobalResouceType type_) :
    type(type_)
  {
  }

  ResourceAttributes::ResourceAttributes(std::initializer_list<int> attributes_) :
    attributes(0)
  {
    assert(attributes_.size() == attributes.capacity());
    component_utils::copyFitInitListToFixedVec(attributes, attributes_);
  }
}
namespace resource_component_utils
{
  const int& getAttribute(const anax::Entity& entity, unsigned int attributeId)
  {
    return entity.getComponent<component::ResourceAttributes>().attributes[attributeId];
  }

  const GlobalResource::EGlobalResouceType& getResourceType(const anax::Entity& entity)
  {
    assert(entity.hasComponent<component::ResourceType>());
    return entity.getComponent<component::ResourceType>().type;
  }

  void setAttribute(const anax::Entity& entity, unsigned int attributeId, int value)
  {
    entity.getComponent<component::ResourceAttributes>().attributes[attributeId] = value;
  }
}
}
