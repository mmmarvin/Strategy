#ifndef NONCOMPRESSEDARCHIVE_DEF_H
#define NONCOMPRESSEDARCHIVE_DEF_H

#include <cinttypes>

namespace gengine2d
{
  // header signiture
  static constexpr std::int_least8_t GEARC_ARCHIVE_01 = 119;
  static constexpr std::int_least8_t GEARC_ARCHIVE_02 = 116;
  static constexpr std::int_least8_t GEARC_MAGIC_01 = 15;
  static constexpr std::int_least8_t GEARC_MAGIC_02 = 50;
  static constexpr std::int_least8_t GEARC_MAGIC_03 = 37;
  static constexpr std::int_least8_t GEARC_MAGIC_04 = 88;

  // header version signiture
  static constexpr std::int_least8_t GEARC_VERSION_01 = 0;
  static constexpr std::int_least8_t GEARC_VERSION_02 = 2;
  static constexpr std::int_least8_t GEARC_VERSION_03 = 0;
  static constexpr std::int_least8_t GEARC_VERSION_04 = 0;

  static constexpr std::int_least8_t GEARC_VERSION_MIN_01 = 0;
  static constexpr std::int_least8_t GEARC_VERSION_MIN_02 = 2;
  static constexpr std::int_least8_t GEARC_VERSION_MIN_03 = 0;
  static constexpr std::int_least8_t GEARC_VERSION_MIN_04 = 0;

  // read lengths
  static constexpr std::size_t GEARC_FILENAME_LENGTH = 200;
  static constexpr std::size_t GEARC_BUFF_LENGTH = 255;
}

#endif // NONCOMPRESSEDARCHIVE_DEF_H
