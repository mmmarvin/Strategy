#include "container_utility.h"

#include <iterator>

namespace gengine2d
{
namespace container_utils
{
  template<class ContainerT>
  void shrinkFrontToFitSize(ContainerT& container, std::size_t size)
  {
    while(container.size() > size) {
      container.erase(container.begin());
    }
  }

  template<class ContainerT>
  void shrinkBackToFitSize(ContainerT& container, std::size_t size)
  {
    while(container.size() > size) {
      container.pop_back();
    }
  }

  template<class ContainerT>
  std::size_t swapAndPop(ContainerT& container, std::size_t index)
  {
    std::swap(container[index], container.back());
    container.pop_back();
    return container.size();
  }
}
}
