#ifndef CONTROLMAP_H
#define CONTROLMAP_H

#include <functional>
#include "type.h"

namespace gengine2d
{
  class GameState;
  class ControlMap
  {
    using control_mapped_type = std::tuple<string_t, bool, unsigned int, std::function<void(GameState*)>>;

  public:
    explicit ControlMap(std::initializer_list<control_mapped_type> initialKeysMapped);

    bool setKeyMapped(const string_t& controlName, bool isMouse, unsigned int key);
    template<class Func> bool forEachKeyMapped(Func&& func) const;

  private:
    fast_unordered_map_t<string_t, std::size_t> m_controlMap;
    vector_t<control_mapped_type>               m_control;
  };
}

#include "controlmap.inl"

#endif // CONTROLMAP_H
