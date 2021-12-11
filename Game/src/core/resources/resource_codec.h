#ifndef RESOURCE_CODEC_H
#define RESOURCE_CODEC_H

#include "../type.h"

namespace gengine2d
{
  vector_t<unsigned char> encode(vector_t<string_t> base64String);
  vector_t<unsigned char> decode(vector_t<string_t> base64String);
  vector_t<unsigned char> encode(vector_t<unsigned char> base64String);
  vector_t<unsigned char> decode(vector_t<unsigned char> base64String);
}

#endif // RESOURCE_CODEC_H
