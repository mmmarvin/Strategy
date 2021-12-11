#ifndef ANIMATION2D_H
#define ANIMATION2D_H

#include "game_internal_types.h"

namespace gengine2d
{
  class Animation2D
  {
  public:
    explicit Animation2D(std::initializer_list<Rectf> frames,
                         float duration,
                         const game_internal::Renderer::Texture* texture);
    explicit Animation2D(vector_t<Rectf> frames,
                         float duration,
                         const game_internal::Renderer::Texture* texture);

    const Rectf& getFrame(std::size_t frameNumber) const;
    std::size_t getFrameCount() const;

    const game_internal::Renderer::Texture* getTexture() const;
    float getDuration() const;

  private:
    vector_t<Rectf>                         m_frames;
    const game_internal::Renderer::Texture* m_texture;
    float                                   m_duration;
  };
}

#endif // ANIMATION2D_H
