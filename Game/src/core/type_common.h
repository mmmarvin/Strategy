#ifndef TYPE_COMMON_H
#define TYPE_COMMON_H

#include <cinttypes>
#include "macro.h"

DISABLE_WARNING_BEGIN
DISABLE_WARNING("-Wredundant-decls")
void* operator new[](std::size_t size, const char* name, int flags, unsigned debugFlags, const char* file, int line);
void* operator new[](std::size_t size, std::size_t alignment, std::size_t alignmentOffset, const char* pName, int flags, unsigned debugFlags, const char* file, int line);
DISABLE_WARNING_END

#endif // TYPE_COMMON_H
