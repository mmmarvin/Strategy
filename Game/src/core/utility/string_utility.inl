#include "string_utility.h"

namespace gengine2d
{
  template<class Func>
  vector_t<string_t> tokenizeIf(string_t str, Func&& func)
  {
    vector_t<string_t> ret;

    std::size_t i = 0;
    while(i < str.size()) {
      while(i < str.size() && std::forward<Func>(func)(str[i])) {
        ++i;
      }

      std::size_t j = i;
      while(j < str.size() && !std::forward<Func>(func)(str[j])) {
        ++j;
      }

      if(i < j) {
        ret.push_back(str.substr(i, j - i));;
        i = j;
      }
    }

    return ret;
  }
}
