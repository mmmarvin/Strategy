#include "animationset2d.h"

#include "utility/stl_utility.h"

namespace gengine2d
{
  AnimationSet2D::AnimationSet2D(std::initializer_list<std::pair<string_t, Animation2D>> animations) :
    m_animations(stl_utils::begin(animations), stl_utils::end(animations))
  {
  }

  AnimationSet2D::AnimationSet2D(vector_t<std::pair<string_t, Animation2D>> animations) :
    m_animations(stl_utils::begin(animations), stl_utils::end(animations))
  {
  }

  const Animation2D& AnimationSet2D::getAnimation(const string_t& animationName) const
  {
    assert(m_animations.find(animationName) != m_animations.end());
    return m_animations.at(animationName);
  }
}
