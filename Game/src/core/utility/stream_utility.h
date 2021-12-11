#ifndef STREAM_UTILITY_H
#define STREAM_UTILITY_H

#include <ios>
#include <memory>
#include "../type.h"

namespace gengine2d
{
namespace stream_utils
{
  void clearStringStream(std::stringstream& stream, std::ios_base::openmode mode = std::ios_base::in | std::ios_base::out);
  void clearStringStreamBinary(std::stringstream& stream);

  std::shared_ptr<std::stringstream> copyStream(std::istream& stream);
  vector_t<unsigned char> copyStreamToArray(std::istream& stream);
}
}

#endif // STREAM_UTILITY_H
