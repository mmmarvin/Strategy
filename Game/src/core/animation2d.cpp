#include "animation2d.h"

namespace gengine2d
{
  Animation2D::Animation2D(std::initializer_list<Rectf> frames,
                           float duration,
                           const game_internal::Renderer::Texture* texture) :
    m_frames(frames),
    m_texture(texture),
    m_duration(duration)
  {
  }

  Animation2D::Animation2D(vector_t<Rectf> frames,
                           float duration,
                           const game_internal::Renderer::Texture* texture) :
    m_frames(std::move(frames)),
    m_texture(texture),
    m_duration(duration)
  {
  }

  const Rectf& Animation2D::getFrame(std::size_t frameNumber) const
  {
    assert(frameNumber < getFrameCount());
    return m_frames[frameNumber];
  }

  std::size_t Animation2D::getFrameCount() const
  {
    return m_frames.size();
  }

  const game_internal::Renderer::Texture* Animation2D::getTexture() const
  {
    return m_texture;
  }

  float Animation2D::getDuration() const
  {
    return m_duration;
  }
}
