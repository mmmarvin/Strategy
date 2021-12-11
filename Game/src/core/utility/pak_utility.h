#ifndef PAK_UTILITY_H
#define PAK_UTILITY_H

#include "../type.h"

namespace gengine2d
{
namespace pak_utils
{
  string_t getTextureName(const string_t& textureFilename,
                          const string_t& texturePackFilename);
}
}

#endif // PAK_UTILITY_H
