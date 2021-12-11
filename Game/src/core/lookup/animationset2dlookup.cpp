#include "animationset2dlookup.h"

#include "../utility/converter_utility.h"
#include "../utility/stream_utility.h"
#include "../utility/pak_utility.h"
#include "../utility/type_determine_utility.h"
#include "texturelookup.h"

namespace gengine2d
{
  AnimationSet2DLookup::AnimationSet2DLookup(TextureLookup& textureLookup) :
    m_textureLookup(&textureLookup)
  {
  }

  bool AnimationSet2DLookup::loadFromStreamImpl(const string_t&, std::istream& stream)
  {
    archive_t animationPak;
    if(animationPak.open(stream) == archive_t::EOR_SUCCESS){
      vector_t<string_t> addedResources;

      for(const auto& filename : animationPak.getFileList()) {
        if(!this->exists(filename)) {
          auto animationSet = loadAnimationSet(animationPak.getFile(filename));
          if(!animationSet || !this->addResource(animationSet.value().first,
                                                 std::move(animationSet.value().second))) {
            for(const auto& resource : addedResources) {
              this->removeResource(resource);
            }
            return false;
          } else {
            addedResources.push_back(filename);
          }
        }
      }

      return true;
    }

    return false;
  }

  bool AnimationSet2DLookup::loadFromStreamIndividualImpl(const string_t& name, const string_t&, std::istream& stream)
  {
    if(!this->exists(name)) {
      archive_t animationPak;
      if(animationPak.open(stream) == archive_t::EOR_SUCCESS){
        if(animationPak.fileExists(name)) {
          auto animationSet = loadAnimationSet(animationPak.getFile(name));
          if(animationSet && this->addResource(animationSet.value().first, std::move(animationSet.value().second))) {
            return true;
          }
        }
      }
    }

    return false;
  }

  std::optional<std::pair<string_t, AnimationSet2D>> AnimationSet2DLookup::loadAnimationSet(archive_file_t* animationSetFile)
  {
    auto animationSetBinData = stream_utils::copyStreamToArray(*animationSetFile->getInputStream());
    if(!animationSetBinData.empty()) {
      tinyxml2::XMLDocument doc;
      if(doc.Parse(reinterpret_cast<const char*>(animationSetBinData.data()),
                   animationSetBinData.size()) == tinyxml2::XMLError::XML_SUCCESS) {
        auto* animationSection = doc.FirstChildElement("animation");
        if(animationSection) {
          auto* animationName = animationSection->Attribute("name");
          if(animationName) {
            vector_t<std::pair<string_t, Animation2D>> animations;

            auto* framesSection = animationSection->FirstChildElement("frames");
            while(framesSection) {
              auto animation = loadAnimation(framesSection);
              if(animation) {
                animations.emplace_back(animation.value().first, animation.value().second);
              }

              framesSection = framesSection->NextSiblingElement("frames");
            }

            if(!animations.empty()) {
              return std::make_pair(animationName,
                                    AnimationSet2D(std::move(animations)));
            }
          }
        }
      }
    }

    return std::nullopt;
  }

  std::optional<std::pair<string_t, Animation2D>> AnimationSet2DLookup::loadAnimation(tinyxml2::XMLElement* framesSection)
  {
    auto name = framesSection->Attribute("name");
    auto duration = framesSection->Attribute("duration");
    auto texture = framesSection->Attribute("texture");
    auto texturePak = framesSection->Attribute("texturePak");

    if(name && duration && texture && texturePak) {
      if(type_determine_utils::isFloatingPoint(duration)) {
        vector_t<Rectf> frames;

        auto* frameSection = framesSection->FirstChildElement("frame");
        while(frameSection) {
          auto* frameX = frameSection->FirstChildElement("frame_x");
          auto* frameY = frameSection->FirstChildElement("frame_y");
          auto* frameWidth = frameSection->FirstChildElement("frame_width");
          auto* frameHeight = frameSection->FirstChildElement("frame_height");
          if(frameX && frameY && frameWidth && frameHeight) {
            if(type_determine_utils::isFloatingPoint(frameX->GetText()) &&
               type_determine_utils::isFloatingPoint(frameY->GetText()) &&
               type_determine_utils::isFloatingPoint(frameWidth->GetText()) &&
               type_determine_utils::isFloatingPoint(frameHeight->GetText())) {
              frames.emplace_back(converter_utils::stringTo<float>(frameX->GetText()),
                                  converter_utils::stringTo<float>(frameY->GetText()),
                                  converter_utils::stringTo<float>(frameWidth->GetText()),
                                  converter_utils::stringTo<float>(frameHeight->GetText()));
            }
          }

          frameSection = frameSection->NextSiblingElement("frame");
        }

        if(!frames.empty()) {
          const game_internal::Renderer::Texture* frameTexture;
          auto textureName = pak_utils::getTextureName(texture, texturePak);

          if(m_textureLookup->exists(textureName)) {
            frameTexture = &m_textureLookup->getResource(textureName);
          } else {
            if(!m_textureLookup->loadIndividual(texture, texturePak)) {
              return std::nullopt;
            }

            frameTexture = &m_textureLookup->getResource(textureName);
          }

          return std::make_pair(name, Animation2D(frames,
                                                  converter_utils::stringTo<float>(duration),
                                                  frameTexture));
        }
      }
    }

    return std::nullopt;
  }
}
