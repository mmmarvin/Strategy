#ifndef ANIMATIONSET2DLOOKUP_H
#define ANIMATIONSET2DLOOKUP_H

#include <optional>
#include "../3rdparty/tinyxml/tinyxml2.h"
#include "../archive_type.h"
#include "../animationset2d.h"
#include "../loadableresourcelookup.h"

namespace gengine2d
{
  class TextureLookup;
  class AnimationSet2DLookup : public LoadableResourceLookup<AnimationSet2D>
  {
  public:
    explicit AnimationSet2DLookup(TextureLookup& textureLookup);

  private:
    bool loadFromStreamImpl(const string_t& filename, std::istream &stream) override;
    bool loadFromStreamIndividualImpl(const string_t& name, const string_t& filename, std::istream& stream) override;

    std::optional<std::pair<string_t, AnimationSet2D>> loadAnimationSet(archive_file_t* animationSetFile);
    std::optional<std::pair<string_t, Animation2D>> loadAnimation(tinyxml2::XMLElement* framesSection);

    TextureLookup* m_textureLookup;
  };
}

#endif // ANIMATIONSET2DLOOKUP_H
