#ifndef FOG_H
#define FOG_H

#include "../core/spatialhash.h"

namespace strat
{
  // TODO: Change this to an array of integer instead of
  // hash? Which is better for performance?
  class Fog
  {
  public:
    Fog() {}

    bool init(std::size_t width, std::size_t height, std::uint8_t alpha = 255);

    void draw();
    void expose(float x, float y);
    void exposeRadius(float x, float y, float radius);

    bool isExposed(float x, float y) const;

  private:
    boost::unordered_map<std::pair<float, float>, int>  m_fog;
  };
}

#endif // FOG_H
