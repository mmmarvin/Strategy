#include "type_common.h"

void* operator new[](std::size_t size, const char*, int, unsigned, const char*, int)
{
  return new uint8_t[size];
}

void* operator new[](std::size_t size, std::size_t, std::size_t, const char*, int, unsigned, const char*, int)
{
  return new uint8_t[size];
}
