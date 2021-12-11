#include "stream_utility.h"

#include <sstream>
#include "../define.h"

namespace gengine2d
{
namespace stream_utils
{
  void clearStringStream(std::stringstream& stream, std::ios_base::openmode mode)
  {
    std::stringstream temp(mode);
    std::swap(stream, temp);
  }

  void clearStringStreamBinary(std::stringstream& stream)
  {
    std::stringstream temp(std::ios_base::in |
                           std::ios_base::out |
                           std::ios_base::binary);
    std::swap(stream, temp);
  }

  std::shared_ptr<std::stringstream> copyStream(std::istream& stream)
  {
    std::shared_ptr<std::stringstream> ret;

    char buff[STREAM_BUFFER_SIZE];
    stream.read(&buff[0], STREAM_BUFFER_SIZE);
    auto read = stream.gcount();
    while(read > 0) {
      ret->write(&buff[0], read);

      stream.read(&buff[0], STREAM_BUFFER_SIZE);
      read = stream.gcount();
    }

    return ret;
  }

  vector_t<unsigned char> copyStreamToArray(std::istream& stream)
  {
    vector_t<unsigned char> ret;

    char buff[255];
    stream.read(&buff[0], sizeof buff);
    auto read = stream.gcount();
    while(read) {
      auto* buffPtr = reinterpret_cast<unsigned char*>(&buff[0]);
      ret.insert(ret.end(), buffPtr, buffPtr + read);

      stream.read(&buff[0], sizeof buff);
      read = stream.gcount();
    }

    return ret;
  }
}
}
