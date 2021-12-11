#ifndef JSON_UTILITY_H
#define JSON_UTILITY_H

#include <iosfwd>
#include <jsonxx/jsonxx.h>
#include "../../core/type.h"
#include "../../core/rect.h"
#include "../../core/vector2.h"

namespace strat
{
namespace json_utils
{
  void outputStartObject(std::ostream& stream);
  void outputStartObject(std::ostream& stream, const gengine2d::string_t& fieldName);
  void outputEndObject(std::ostream& stream);
  void outputStartArray(std::ostream& stream);
  void outputStartArray(std::ostream& stream, const gengine2d::string_t& fieldName);
  void outputEndArray(std::ostream& stream);

  template<class InputIteratorT, class Func>
  void outputArray(std::ostream& stream, InputIteratorT first, InputIteratorT last, Func&& func);
  template<class InputIteratorT, class Func>
  void outputArray(std::ostream& stream,
                  const gengine2d::string_t& fieldName,
                  InputIteratorT first,
                  InputIteratorT last,
                  Func&& func);

  void outputString(std::ostream& stream, const gengine2d::string_t& fieldName, const gengine2d::string_t& str);
  void outputNumber(std::ostream& stream, const gengine2d::string_t& fieldName, double number);
  void outputBoolean(std::ostream& stream, const gengine2d::string_t& fieldName, bool boolean);
  void outputVector2(std::ostream& stream, const gengine2d::string_t& fieldName, const gengine2d::Vector2<double>& vec);
  void outputRect(std::ostream& stream, const gengine2d::string_t& fieldName, const gengine2d::Rect<double>& rect);
  void outputComma(std::ostream& stream);

  bool hasVector2(const jsonxx::Object& object, const gengine2d::string_t& fieldName);
  bool hasRect(const jsonxx::Object& object, const gengine2d::string_t& fieldName);

  template<class T>
  gengine2d::Vector2<T> getVector2(const jsonxx::Object& object, const gengine2d::string_t& fieldName);
  template<class T>
  gengine2d::Rect<T> getRect(const jsonxx::Object& object, const gengine2d::string_t& fieldName);
}
}

#include "json_utility.inl"

#endif // JSON_UTILITY_H
