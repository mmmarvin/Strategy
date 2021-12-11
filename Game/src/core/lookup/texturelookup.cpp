#include "texturelookup.h"

#include "../utility/pak_utility.h"
#include "../game.h"

namespace gengine2d
{
  const game_internal::Renderer::Texture& TextureLookup::getResource(const string_t& name) const
  {
    if(this->exists(name)) {
      return LoadableResourceLookup<game_internal::Renderer::Texture>::getResource(name);
    }

    return GameEngine::getInstance().getResourceManager().getTextureLookup().getResource("TEXTURE_DEFAULT");
  }

  bool TextureLookup::loadFromStreamImpl(const string_t& filename, std::istream& stream)
  {
    archive_t texturePak;
    if(texturePak.open(stream) == archive_t::EOR_SUCCESS) {
      vector_t<string_t> addedResources;

      for(const auto& textureFilename : texturePak.getFileList()) {
        auto textureName = pak_utils::getTextureName(textureFilename, filename);
        if(!this->exists(textureName)) {
          auto texture = loadTexture(texturePak.getFile(textureFilename ));
          if(!texture || !this->addResource(textureName, std::move(texture.value()))) {
            for(const auto& resource : addedResources) {
              this->removeResource(resource);
            }
            return false;
          } else {
            addedResources.push_back(textureName);
          }
        }
      }

      return true;
    }

    return false;
  }

  bool TextureLookup::loadFromStreamIndividualImpl(const string_t& name,
                                                   const string_t& filename,
                                                   std::istream& stream)
  {
    if(!this->exists(name)) {
      archive_t texturePak;
      if(texturePak.open(stream) == archive_t::EOR_SUCCESS &&
         texturePak.fileExists(name)) {
        auto texture = loadTexture(texturePak.getFile(name));
        if(texture && this->addResource(pak_utils::getTextureName(name , filename),
                                        std::move(texture.value()))) {
          return true;
        }
      }
    }

    return false;
  }

  std::optional<game_internal::Renderer::Texture> TextureLookup::loadTexture(archive_file_t* textureFile)
  {
    auto texture = GameEngine::getInstance().getRenderer().loadTexture("", *textureFile->getInputStream());
    if(texture.first) {
      return texture.second;
    }

    return std::nullopt;
  }
}
