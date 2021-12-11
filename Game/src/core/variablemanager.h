#ifndef VARIABLEMANAGER_H
#define VARIABLEMANAGER_H

#include <cassert>
#include <variant>
#include "type.h"

namespace gengine2d
{
  enum class EVariableType : unsigned char
  {
    Boolean,
    Integer,
    FloatingPoint,
    String
  };

  class VariableManager
  {
    struct Variable
    {
      using value_type = std::variant<bool, std::int64_t, double, string_t>;

      Variable(value_type&& v);

      value_type value;
    };

  public:
    enum class ESetReturn : unsigned char
    {
      Success = 0,
      InvalidType,
      InvalidName,
      IsReadOnly
    };

    explicit VariableManager(std::initializer_list<std::tuple<string_t, string_t, EVariableType, bool>> variables = {});

    bool addVariable(string_t name, string_t value, EVariableType type, bool readOnly = false);
    ESetReturn setVariable(const string_t& name, string_t value);
    bool setReadOnly(const string_t& name, bool readOnly);

    bool exists(const string_t& name) const;

    const bool* getBool(const string_t& name) const { return getVariable<bool>(name); }
    const std::int64_t* getInteger(const string_t& name) const { return getVariable<std::int64_t>(name); }
    const double* getFloat(const string_t& name) const { return getVariable<double>(name); }
    const string_t* getString(const string_t& name) const { return getVariable<string_t>(name); }
    string_t getValueString(const string_t& name) const;

    vector_t<string_t> getVariableList() const;

  private:
    template<class T> const T* getVariable(const string_t& name) const;

    // TODO: Add a function callback to when a value is set
    // ex. when renderer values such as window.width / window.height
    // is changed, the callback function will update the renderer window size
    // on set
    fast_unordered_map_t<string_t, std::tuple<Variable, EVariableType, bool>> m_variables;
  };

  template<class T>
  const T* VariableManager::getVariable(const string_t& name) const
  {
    auto findIt = m_variables.find(name);
    assert(findIt != m_variables.end());

    if(findIt != m_variables.end()) {
      const T* ret = std::get_if<T>(&std::get<0>(findIt->second).value);
      assert(ret);

      return ret;
    }

    return nullptr;
  }
}

#endif // VARIABLEMANAGER_H
