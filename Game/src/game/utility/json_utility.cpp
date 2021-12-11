#include "json_utility.h"

#include "../../core/debug/debug_log.h"

namespace strat
{
namespace json_utils
{
namespace
{
  static int& getOutputTabIndex()
  {
    static int outputTabIndex = 0;
    return outputTabIndex;
  }

  void outputWriteTabs(std::ostream& stream)
  {
    for(int i = 0, isize = getOutputTabIndex(); i < isize; ++i) {
      stream << "\t";
    }
  }
}
  void outputStartObject(std::ostream& stream)
  {
    outputWriteTabs(stream);
    stream << "{\n";
    ++getOutputTabIndex();
  }

  void outputStartObject(std::ostream& stream, const gengine2d::string_t& fieldName)
  {
    outputWriteTabs(stream);
    stream << "\"" << fieldName << "\": {\n";
    ++getOutputTabIndex();
  }

  void outputEndObject(std::ostream& stream)
  {
    stream << "\n";
    --getOutputTabIndex();
    outputWriteTabs(stream);
    stream << "}";
  }

  void outputStartArray(std::ostream& stream)
  {
    outputWriteTabs(stream);
    stream << "[\n";
    ++getOutputTabIndex();
  }

  void outputStartArray(std::ostream& stream, const gengine2d::string_t& fieldName)
  {
    outputWriteTabs(stream);
    stream << "\"" << fieldName << "\": [\n";
    ++getOutputTabIndex();
  }

  void outputEndArray(std::ostream& stream)
  {
    stream << "\n";
    --getOutputTabIndex();
    outputWriteTabs(stream);
    stream << "]";
  }

  void outputString(std::ostream& stream, const gengine2d::string_t& fieldName, const gengine2d::string_t& str)
  {
    outputWriteTabs(stream);
    stream << "\"" << fieldName << "\": \"" << str << "\"";
  }

  void outputNumber(std::ostream& stream, const gengine2d::string_t& fieldName, double number)
  {
    outputWriteTabs(stream);
    stream << "\"" << fieldName << "\": " << number;
  }

  void outputBoolean(std::ostream& stream, const gengine2d::string_t& fieldName, bool boolean)
  {
    outputWriteTabs(stream);
    stream << "\"" << fieldName << "\": " << (boolean ? gengine2d::string_t("true") : gengine2d::string_t("false"));
  }

  void outputVector2(std::ostream& stream, const gengine2d::string_t& fieldName, const gengine2d::Vector2<double>& vec)
  {
    outputWriteTabs(stream);
    stream << "\"" << fieldName << "\": [" << vec.x << ", " << vec.y << "]";
  }

  void outputRect(std::ostream& stream, const gengine2d::string_t& fieldName, const gengine2d::Rect<double>& rect)
  {
    outputWriteTabs(stream);
    stream << "\""
           << fieldName
           << "\": ["
           << rect.left
           << ", "
           << rect.top
           << ", "
           << rect.width
           << ", "
           << rect.height
           <<  "]";
  }

  void outputComma(std::ostream& stream)
  {
    stream << ",\n";
  }

  bool hasVector2(const jsonxx::Object& object, const gengine2d::string_t& fieldName)
  {
    if(object.has<jsonxx::Array>(fieldName)) {
      const auto& arr = object.get<jsonxx::Array>(fieldName);
      return arr.size() == 2 && arr.has<jsonxx::Number>(0) && arr.has<jsonxx::Number>(1);
    }

    return false;
  }

  bool hasRect(const jsonxx::Object& object, const gengine2d::string_t& fieldName)
  {
    if(object.has<jsonxx::Array>(fieldName)) {
      const auto& arr = object.get<jsonxx::Array>(fieldName);
      return arr.size() == 4 &&
             arr.has<jsonxx::Number>(0) &&
             arr.has<jsonxx::Number>(1) &&
             arr.has<jsonxx::Number>(2) &&
             arr.has<jsonxx::Number>(3);
    }

    return false;
  }
}
}
