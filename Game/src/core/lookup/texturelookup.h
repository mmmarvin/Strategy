#ifndef TEXTURELOOKUP_H
#define TEXTURELOOKUP_H

#include <optional>
#include "../archive_type.h"
#include "../game_internal_types.h"
#include "../loadableresourcelookup.h"

namespace gengine2d
{
  class TextureLookup : public LoadableResourceLookup<game_internal::Renderer::Texture>
  {
  public:
    TextureLookup() {}

    const game_internal::Renderer::Texture& getResource(const string_t& name) const;

  private:
    bool loadFromStreamImpl(const string_t& filename, std::istream& stream) override;
    bool loadFromStreamIndividualImpl(const string_t& name,
                                      const string_t& filename,
                                      std::istream& stream) override;

    std::optional<game_internal::Renderer::Texture> loadTexture(archive_file_t* textureFile);
  };
}

#endif // TEXTURELOOKUP_H
