#include "controlmap.h"

namespace gengine2d
{
  template<class Func>
  bool ControlMap::forEachKeyMapped(Func&& func) const
  {
    for(const auto& control_mapped : m_control) {
      if(std::forward<Func>(func)(std::get<0>(control_mapped),
                                  std::get<1>(control_mapped),
                                  std::get<2>(control_mapped),
                                  std::get<3>(control_mapped))) {
        return true;
      }
    }

    return false;
  }
}
