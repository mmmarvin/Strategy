#include "variablemanager.h"

#include <sstream>
#include "debug/assert_result.h"
#include "utility/converter_utility.h"
#include "utility/type_determine_utility.h"
#include "utility/string_utility.h"
#include "utility/stl_utility.h"

namespace gengine2d
{
  VariableManager::Variable::Variable(value_type&& v) :
    value(std::move(v))
  {
  }

  VariableManager::VariableManager(std::initializer_list<std::tuple<string_t, string_t, EVariableType, bool>> variables)
  {
    for(auto [name, value, type, readOnly] : variables) {
      assert_result(addVariable(std::move(name), std::move(value), type, readOnly));
    }
  }

  bool VariableManager::addVariable(string_t name, string_t value, EVariableType type, bool readOnly)
  {
    switch(type) {
    case EVariableType::Boolean:
      if(type_determine_utils::isBoolean(value)) {
        m_variables.emplace(std::move(name),
                            std::make_tuple(Variable(Variable::value_type(std::in_place_type_t<bool>(), converter_utils::stringTo<bool>(std::move(value)))),
                                            type,
                                            readOnly));
        return true;
      }
      break;
    case EVariableType::Integer:
      if(type_determine_utils::isInteger(value)) {
        m_variables.emplace(std::move(name),
                            std::make_tuple(Variable(Variable::value_type(std::in_place_type_t<std::int64_t>(), converter_utils::stringTo<std::int64_t>(std::move(value)))),
                                            type,
                                            readOnly));
        return true;
      }
      break;
    case EVariableType::FloatingPoint:
      if(type_determine_utils::isFloatingPoint(value)) {
        m_variables.emplace(std::move(name),
                            std::make_tuple(Variable(Variable::value_type(std::in_place_type_t<double>(), converter_utils::stringTo<double>(std::move(value)))),
                                            type,
                                            readOnly));
        return true;
      }
      break;
    case EVariableType::String:
      value = tryRemoveQuotations(std::move(value));
      m_variables.emplace(std::move(name),
                          std::make_tuple(Variable(Variable::value_type(std::in_place_type_t<string_t>(), std::move(value))),
                                          type,
                                          readOnly));
      return true;
    default:
      break;
    }

    return false;
  }

  VariableManager::ESetReturn VariableManager::setVariable(const string_t& name, string_t value)
  {
    auto findIt = m_variables.find(name);
    if(findIt != m_variables.end()) {
      if(std::get<2>(findIt->second)) {
        return ESetReturn::IsReadOnly;
      }

      auto type = std::get<1>(findIt->second);
      switch(type) {
      case EVariableType::Boolean:
        if(type_determine_utils::isBoolean(value)) {
          std::get<0>(findIt->second) = Variable::value_type(std::in_place_type_t<bool>(),
                                                             converter_utils::stringTo<bool>(std::move(value)));
          return ESetReturn::Success;
        } else
          return ESetReturn::InvalidType;
        break;
      case EVariableType::Integer:
        if(type_determine_utils::isInteger(value)) {
          std::get<0>(findIt->second) = Variable::value_type(std::in_place_type_t<std::int64_t>(),
                                                             converter_utils::stringTo<std::int64_t>(std::move(value)));
          return ESetReturn::Success;
        } else
          return ESetReturn::InvalidType;
        break;
      case EVariableType::FloatingPoint:
        if(type_determine_utils::isFloatingPoint(value)) {
          std::get<0>(findIt->second) = Variable::value_type(std::in_place_type_t<double>(),
                                                             converter_utils::stringTo<double>(std::move(value)));
          return ESetReturn::Success;
        } else
          return ESetReturn::InvalidType;
        break;
      case EVariableType::String:
        value = tryRemoveQuotations(std::move(value));
        std::get<0>(findIt->second) = Variable::value_type(std::in_place_type_t<string_t>(),
                                                           std::move(value));
        return ESetReturn::Success;
      default:
        break;
      }
    }

    return ESetReturn::InvalidName;
  }

  bool VariableManager::setReadOnly(const string_t& name, bool readOnly)
  {
    auto findIt = m_variables.find(name);
    if(findIt == m_variables.end()) {
      std::get<2>(findIt->second) = readOnly;
      return true;
    }

    return false;
  }

  bool VariableManager::exists(const string_t& name) const
  {
    return m_variables.find(name) != m_variables.end();
  }

  string_t VariableManager::getValueString(const string_t& name) const
  {
    auto findIt = m_variables.find(name);
    if(findIt != m_variables.end()) {
      auto type = std::get<1>(findIt->second);
      switch(type) {
      case EVariableType::Boolean:
        return stl_utils::to_string(*getBool(name));
      case EVariableType::Integer:
        return stl_utils::to_string(*getInteger(name));
      case EVariableType::FloatingPoint:
        return stl_utils::to_string(*getFloat(name));
      case EVariableType::String:
        return *getString(name);
      default:
        break;
      }
    }

    return "";
  }

  vector_t<string_t> VariableManager::getVariableList() const
  {
    vector_t<string_t> ret;
    for(const auto& [key, value] : m_variables) {
      ret.push_back(key);
    }

    return ret;
  }
}
