#include "type_determine_utility.h"

#include <cctype>
#include "stl_utility.h"
#include "string_utility.h"

namespace gengine2d
{
namespace type_determine_utils
{
namespace
{
  bool isIntegerImpl(const string_t& str)
  {
    bool neg = false;
    for(auto c : str) {
      if(!isdigit(c)) {
        if(c == '-') {
          if(!neg) {
            neg = true;
          } else {
            return false;
          }
        }
      }
    }

    return true;
  }

  bool isFloatingPointImpl(const string_t& str)
  {
    bool neg = false, point = false;
    for(auto c : str) {
      if(!isdigit(c)) {
        if(c == '-') {
          if(!neg) {
            neg = true;
          } else {
            return false;
          }
        } else if(c == '.') {
          if(!point) {
            point = true;
          } else {
            return false;
          }
        }
      }
    }

    return true;
  }
}
  bool isBoolean(const string_t& str)
  {
    return toLower(str) == "true" || toLower(str) == "false";
  }

  bool isInteger(const string_t& str)
  {
    static const auto MAX_INTEGER_STR = gengine2d::stl_utils::to_string(std::numeric_limits<int>::max());
    static const auto MIN_INTEGER_STR = gengine2d::stl_utils::to_string(std::numeric_limits<int>::min());

    if(std::find(str.begin(), str.end(), '-') != str.end() &&
       str.size() <= MIN_INTEGER_STR.size()) {
      return isIntegerImpl(str);
    } else  if(str.size() <= MAX_INTEGER_STR.size()) {
      return isIntegerImpl(str);
    }

    return false;
  }

  bool isFloatingPoint(const string_t& str)
  {
    if(str.size() <= 7) {
      return isFloatingPointImpl(str);
    }

    if(str.size() == 8 &&
       (std::find(str.begin(), str.end(), '-') != str.end() ||
        std::find(str.begin(), str.end(), '.') != str.end())) {
      return isFloatingPointImpl(str);
    }

    if(str.size() == 9 &&
       std::find(str.begin(), str.end(), '-') != str.end() &&
        std::find(str.begin(), str.end(), '.') != str.end()) {
      return isFloatingPointImpl(str);
    }

    return false;
  }
}
}
