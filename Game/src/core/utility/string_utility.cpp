#include "string_utility.h"

#include <sstream>
#include "../debug/debug_log.h"

namespace gengine2d
{
namespace
{
  bool partialMatchSeekForward(const string_t& substr, const string_t& str)
  {
    auto it1 = substr.begin();
    auto it1End = substr.end();
    auto it2 = str.begin();
    for(; it1 != it1End; ) {
      if(*it1++ != *it2++) {
        return false;
      }
    }

    return true;
  }

  bool partialMatchSeekBackward(const string_t& substr, const string_t& str)
  {
    auto it1 = substr.rbegin();
    auto it1End = substr.rend();
    auto it2 = str.rbegin();
    for(; it1 != it1End; ) {
      if(*it1++ != *it2++) {
        return false;
      }
    }

    return true;
  }
}
  string_t toLower(string_t str)
  {
    std::transform(str.begin(), str.end(), str.begin(), [](char c) {
      return static_cast<char>(tolower(static_cast<unsigned char>(c)));
    });

    return str;
  }

  string_t toUpper(string_t str)
  {
    std::transform(str.begin(), str.end(), str.begin(), [](char c) {
      return static_cast<char>(toupper(static_cast<unsigned char>(c)));
    });

    return str;
  }

  vector_t<string_t> tokenize(string_t str, vector_t<char> delimiters)
  {
    vector_t<string_t> ret;
    std::size_t i = 0;
    while(i < str.size()) {
      while(i < str.size() && isspace(str[i])) {
        ++i;
      }

      char endChar = ' ';
      auto firstChar = str[i];
      for(auto c : delimiters) {
        if(firstChar == c) {
          endChar = firstChar;
          ++i;
          break;
        }
      }

      auto j = i;
      while(j < str.size() && str[j] != endChar) {
        ++j;
      }

      if(i < j) {
        ret.push_back(str.substr(i, j - i));
        i = j + 1;
      }
    }

    return ret;
  }

  vector_t<string_t> tokenize2(string_t str, char delimiter)
  {
    return tokenizeIf(str, [delimiter](char c) { return c == delimiter; });
  }

  string_t combineTokens(const vector_t<string_t>& tokens,
                         char delimiter,
                         std::size_t start,
                         std::size_t end)
  {
    string_t ret;
    if(start < tokens.size()) {
      ret += tokens[start];
      for(std::size_t i = start + 1; i < end; ++i) {
        ret.push_back(delimiter);
        ret += tokens[i];
      }
    }

    return ret;
  }

  wstring_t toWideString(string_t str)
  {
    return wstring_t(str.begin(), str.end());
  }

  string_t toNarrowString(wstring_t str)
  {
    return string_t(str.begin(), str.end());
  }

  string_t tryRemoveQuotations(string_t str)
  {
    if(str.size() > 2 && str.front() == '\"' && str.back() == '\"') {
      str.erase(str.begin());
      str.pop_back();
    }

    return str;
  }

  bool partialMatch(const string_t& substr, const string_t& str, ESeekDirection dir)
  {
    if(substr.size() <= str.size()) {
      if(dir == ESeekDirection::SD_BEG) {
        return partialMatchSeekForward(substr, str);
      } else if(dir == ESeekDirection::SD_END) {
        return partialMatchSeekBackward(substr, str);
      }
    }

    return false;
  }

  bool matchBeginning(const string_t& substr, const string_t& str)
  {
    return partialMatch(substr, str, ESeekDirection::SD_BEG);
  }

  bool matchEnding(const string_t& substr, const string_t& str)
  {
    return partialMatch(substr, str, ESeekDirection::SD_END);
  }
}
