#include "resource_codec.h"

#include <cppcodec/base64_rfc4648.hpp>

namespace gengine2d
{
  vector_t<unsigned char> encode(vector_t<string_t> base64String)
  {
    vector_t<unsigned char> ret;
    for(const auto& line : base64String) {
      auto encodedChunk = cppcodec::base64_rfc4648::encode<vector_t<unsigned char>>(line.data(), line.size());
      ret.insert(ret.end(), encodedChunk.begin(), encodedChunk.end());
    }

    return ret;
  }

  vector_t<unsigned char> decode(vector_t<string_t> base64String)
  {
    vector_t<unsigned char> ret;
    for(const auto& line : base64String) {
      auto decodedChunk = cppcodec::base64_rfc4648::decode<vector_t<unsigned char>>(line.data(), line.size());
      ret.insert(ret.end(), decodedChunk.begin(), decodedChunk.end());
    }

    return ret;
  }

  vector_t<unsigned char> encode(vector_t<unsigned char> base64String)
  {
    return cppcodec::base64_rfc4648::encode<vector_t<unsigned char>>(base64String.data(), base64String.size());
  }

  vector_t<unsigned char> decode(vector_t<unsigned char> base64String)
  {
    return cppcodec::base64_rfc4648::decode<vector_t<unsigned char>>(reinterpret_cast<char*>(base64String.data()), base64String.size());;
  }
}
