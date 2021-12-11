#ifndef RESOURCE_COMPONENTS_H
#define RESOURCE_COMPONENTS_H

#include <anax/anax.hpp>
#include "../../../core/type.h"
#include "../../global_resource.h"

namespace strat
{
namespace component
{
  struct ResourceType : public anax::Component
  {
    explicit ResourceType(GlobalResource::EGlobalResouceType type_);

    GlobalResource::EGlobalResouceType type;
  };

  struct ResourceAttributes : public anax::Component
  {
    enum : unsigned int
    {
      RA_ATTRIB_RES_VALUE_LEFT = 0,
      RA_ATTRIB_RES_VALUE_MAX,
      RA_NUMBER_OF_ATTRIBS
    };

    explicit ResourceAttributes(std::initializer_list<int> attributes_ = {});

    gengine2d::fixed_vector_t<int, RA_NUMBER_OF_ATTRIBS> attributes;
  };
}
namespace resource_component_utils
{
  const int& getAttribute(const anax::Entity& entity, unsigned int attributeId);
  const GlobalResource::EGlobalResouceType& getResourceType(const anax::Entity& entity);

  void setAttribute(const anax::Entity& entity, unsigned int attributeId, int value);
}
}

#endif // RESOURCE_COMPONENTS_H
