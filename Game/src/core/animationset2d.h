#ifndef ANIMATIONSET2D_H
#define ANIMATIONSET2D_H

#include "animation2d.h"
#include "type.h"

namespace gengine2d
{
  class AnimationSet2D
  {
  public:
    explicit AnimationSet2D(std::initializer_list<std::pair<string_t, Animation2D>> animations);
    explicit AnimationSet2D(vector_t<std::pair<string_t, Animation2D>> animations);

    const Animation2D& getAnimation(const string_t& animationName) const;

  private:
    fast_unordered_map_t<string_t, Animation2D> m_animations;
  };
}

#endif // ANIMATIONSET2D_H
