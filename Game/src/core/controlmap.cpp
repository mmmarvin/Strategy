#include "controlmap.h"

#include <cassert>

namespace gengine2d
{
  ControlMap::ControlMap(std::initializer_list<control_mapped_type> initialKeysMapped)
  {
    for(std::size_t i = 0, isize = initialKeysMapped.size(); i < isize; ++i) {
      const auto& [controlName, isMouse, key, func] = *(initialKeysMapped.begin() + i);
      assert(m_controlMap.find(controlName) == m_controlMap.end());
      m_controlMap.emplace(controlName, i);
      m_control.push_back(std::make_tuple(controlName, isMouse, key, func));
    }
  }

  bool ControlMap::setKeyMapped(const string_t& controlName, bool isMouse, unsigned int key)
  {
    auto findIt = m_controlMap.find(controlName);
    if(findIt != m_controlMap.end()) {
      auto index = findIt->second;
      auto& controlMapped = m_control[index];
      std::get<0>(controlMapped) = isMouse;
      std::get<1>(controlMapped) = key;
      return true;
    }

    return false;
  }
}
