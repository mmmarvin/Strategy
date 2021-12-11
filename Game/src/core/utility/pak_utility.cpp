#include "pak_utility.h"

#include <filesystem>
#include "string_utility.h"

namespace gengine2d
{
namespace pak_utils
{
namespace
{
  string_t getName(const string_t& tag,
                   const string_t& filename,
                   const string_t& packFilename)
  {
    return tag + "_" +
           std::filesystem::path(toLower(packFilename)).filename().replace_extension("").string() + "_" +
           std::filesystem::path(toLower(filename)).filename().replace_extension("").string();
  }
}
  string_t getTextureName(const string_t& textureFilename,
                          const string_t& texturePackFilename)
  {
    return getName("TEXTURE_PAK", textureFilename, texturePackFilename);
  }
}
}
