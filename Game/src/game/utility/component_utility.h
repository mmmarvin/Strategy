#ifndef COMPONENT_UTILITY_H
#define COMPONENT_UTILITY_H

#include "../../core/type.h"

namespace strat
{
namespace component_utils
{
  // NOTE: This assumes that vec already has a fixed size
  template<class T, std::size_t Size>
  void copyFitInitListToFixedVec(gengine2d::fixed_vector_t<T, Size>& vec,
                                 std::initializer_list<T> initList);
  template<class T, std::size_t Size>
  void copyFitInitListToArray(std::array<T, Size>& array,
                              std::initializer_list<T> initList);
}
}

#include "component_utility.inl"

#endif // COMPONENT_UTILITY_H
