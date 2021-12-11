#include "converter_utility.h"

#include <sstream>

namespace gengine2d
{
namespace detail
{
  template<class T>
  struct stringToImpl
  {
    T operator()(string_t str)
    {
      T ret;

      std::stringstream ss;
      ss.str(str);
      ss >> ret;

      return ret;
    }
  };

  template<>
  struct stringToImpl<bool>
  {
    bool operator()(string_t str)
    {
      return str == "true";
    }
  };
}
namespace converter_utils
{
  template<class T>
  T stringTo(string_t str)
  {
    return detail::stringToImpl<T>()(std::move(str));
  }

  template<class T>
  std::size_t to1D(T x, T y, T width)
  {
    return y * width + x;
  }
}
}
