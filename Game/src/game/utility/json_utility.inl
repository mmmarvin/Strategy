#include "json_utility.h"

namespace strat
{
namespace json_utils
{
  template<class T>
  gengine2d::Vector2<T> getVector2(const jsonxx::Object& object, const gengine2d::string_t& fieldName)
  {
    const auto& arr = object.get<jsonxx::Array>(fieldName);
    return gengine2d::Vector2<T>(arr.get<jsonxx::Number>(0), arr.get<jsonxx::Number>(1));
  }

  template<class T>
  gengine2d::Rect<T> getRect(const jsonxx::Object& object, const gengine2d::string_t& fieldName)
  {
    const auto& arr = object.get<jsonxx::Array>(fieldName);
    return gengine2d::Rect<T>(arr.get<jsonxx::Number>(0), arr.get<jsonxx::Number>(1),
                              arr.get<jsonxx::Number>(2), arr.get<jsonxx::Number>(3));
  }

  template<class InputIteratorT, class Func>
  void outputArray(std::ostream& stream, InputIteratorT first, InputIteratorT last, Func&& func)
  {
    outputStartArray(stream);
    for(auto it = first; it != last; ++it) {
      if(it != first) {
        json_utils::outputComma(stream);
      }
      std::forward<Func>(func)(*it);
    }
    outputEndArray(stream);
  }

  template<class InputIteratorT, class Func>
  void outputArray(std::ostream& stream,
                  const gengine2d::string_t& fieldName,
                  InputIteratorT first,
                  InputIteratorT last,
                  Func&& func)
  {
    outputStartArray(stream, fieldName);
    for(auto it = first; it != last; ++it) {
      if(it != first) {
        json_utils::outputComma(stream);
      }
      std::forward<Func>(func)(*it);
    }
    outputEndArray(stream);
  }
}
}
