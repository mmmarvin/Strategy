#include "component_utility.h"

#include "../../core/utility/general_utility.h"

namespace strat
{
namespace component_utils
{
  template<class T, std::size_t Size>
  void copyFitInitListToFixedVec(gengine2d::fixed_vector_t<T, Size>& vec,
                                 std::initializer_list<T> initList)
  {
    auto* beg = initList.begin();
    for(std::size_t i = 0, isize = gengine2d::general_utils::min(vec.capacity(), initList.size()); i < isize; ++i) {
      vec[i] = *(beg++);
    }
  }

  template<class T, std::size_t Size>
  void copyFitInitListToArray(std::array<T, Size>& array,
                              std::initializer_list<T> initList)
  {
    auto* beg = initList.begin();
    for(std::size_t i = 0, isize = gengine2d::general_utils::min(array.size(), initList.size()); i < isize; ++i) {
      array[i] = *(beg++);
    }
  }
}
}
