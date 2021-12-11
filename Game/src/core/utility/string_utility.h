#ifndef STRING_UTILITY_H
#define STRING_UTILITY_H

#include "../base.h"
#include "../type.h"

namespace gengine2d
{
  string_t toLower(string_t str);
  string_t toUpper(string_t str);

  vector_t<string_t> tokenize(string_t str, vector_t<char> delimiters = {});
  vector_t<string_t> tokenize2(string_t str, char delimiter);
  template<class Func> vector_t<string_t> tokenizeIf(string_t str, Func&& func);

  string_t combineTokens(const vector_t<string_t>& tokens,
                         char delimiter,
                         std::size_t start,
                         std::size_t end);

  wstring_t toWideString(string_t str);
  string_t toNarrowString(wstring_t str);

  string_t tryRemoveQuotations(string_t str);

  bool partialMatch(const string_t& substr, const string_t& str, ESeekDirection dir = ESeekDirection::SD_BEG);
  bool matchBeginning(const string_t& substr, const string_t& str);
}

#include "string_utility.inl"

#endif // STRING_UTILITY_H
