#ifndef CONTAINER_UTILITY_H
#define CONTAINER_UTILITY_H

#include <cinttypes>

namespace gengine2d
{
namespace container_utils
{
  template<class ContainerT> void shrinkFrontToFitSize(ContainerT& container, std::size_t size);
  template<class ContainerT> void shrinkBackToFitSize(ContainerT& container, std::size_t size);
  template<class ContainerT> std::size_t swapAndPop(ContainerT& container, std::size_t index);
}
}

#include "container_utility.inl"

#endif // CONTAINER_UTILITY_H
