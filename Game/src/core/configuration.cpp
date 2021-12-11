#include "configuration.h"

#include <fstream>
#include "debug/debug_log.h"
#include "utility/string_utility.h"

namespace gengine2d
{
  Configuration::Configuration() :
    ClassConsoleWriter("CONFIGLoader")
  {
  }

  bool Configuration::loadFromFile(const string_t& filename)
  {
    std::ifstream in(filename);
    if(in.is_open()) {
      return loadFromStream(in);
    }

    return false;
  }

  bool Configuration::loadFromStream(std::istream& stream)
  {
    string_t currentSection = "";
    string_t line;

    while(std::getline(stream, line)) {
      auto tokens = tokenizeIf(line, [](char c) { return isspace(c) || c == '='; });
      if(tokens.size() == 1) {
        auto token = tokens.back();
        if(token.size() > 2 && token.front() == '[' && token.back() == ']') {
          currentSection = token.substr(1, token.size() - 2);
        } else {
          this->printError("Unknown \"" + token + "\"");
        }
      } else if(tokens.size() == 2) {
        auto name = tokens[0];
        auto value = tokens[1];

        auto setResult = setVariable(currentSection, name, value);
        if(setResult == -1) {
          this->printError("Unknown section \"" + currentSection + "\"");
        } else {
          switch(static_cast<VariableManager::ESetReturn>(setResult)) {
          case VariableManager::ESetReturn::Success:
            break;
          case VariableManager::ESetReturn::InvalidType:
            this->printError("\"" + name + "\" is of invalid type");
            break;
          case VariableManager::ESetReturn::InvalidName:
            this->printError("\"" + name + "\" is not a valid field");
            DEBUG_LOG("Invalid name");
            break;
          case VariableManager::ESetReturn::IsReadOnly:
            this->printError("\"" + name + "\" is read only");
            break;
          default:
            break;
          }
        }
      }
    }

    return true;
  }

  bool Configuration::addVariable(const string_t& section,
                                  const string_t& name,
                                  const string_t& value,
                                  EVariableType type)
  {
    if(section.empty() || section == "Universal") {
      return m_universalVariable.addVariable(name, value, type);
    }

    auto findIt = m_variableSections.find(section);
    if(findIt == m_variableSections.end()) {
      auto res = m_variableSections.emplace(section, VariableManager());
      if(res.second) {
        return res.first->second.addVariable(name, value, type);
      } else {
        return false;
      }
    }

    return findIt->second.addVariable(name, value, type);
  }

  const bool* Configuration::getBool(const string_t& section,
                                     const string_t& name) const
  {
    if(section.empty() || section == "Universal") {
      return m_universalVariable.getBool(name);
    }

    if(auto findIt = m_variableSections.find(section); findIt != m_variableSections.end()) {
      return findIt->second.getBool(name);
    }

    return nullptr;
  }

  const std::int64_t* Configuration::getInteger(const string_t& section,
                                                const string_t& name) const
  {
    if(section.empty() || section == "Universal") {
      return m_universalVariable.getInteger(name);
    }

    if(auto findIt = m_variableSections.find(section); findIt != m_variableSections.end()) {
      return findIt->second.getInteger(name);
    }

    return nullptr;
  }

  const double* Configuration::getFloat(const string_t& section,
                                        const string_t& name) const
  {
    if(section.empty() || section == "Universal") {
      return m_universalVariable.getFloat(name);
    }

    if(auto findIt = m_variableSections.find(section); findIt != m_variableSections.end()) {
      return findIt->second.getFloat(name);
    }

    return nullptr;
  }

  const string_t* Configuration::getString(const string_t& section,
                                           const string_t& name) const
  {
    if(section.empty() || section == "Universal") {
      return m_universalVariable.getString(name);
    }

    if(auto findIt = m_variableSections.find(section); findIt != m_variableSections.end()) {
      return findIt->second.getString(name);
    }

    return nullptr;
  }

  int Configuration::setVariable(const string_t& section, const string_t& name, string_t value)
  {
    if(section.empty() || section == "Universal") {
      return static_cast<int>(m_universalVariable.setVariable(name, std::move(value)));
    }

    auto findIt = m_variableSections.find(section);
    if(findIt != m_variableSections.end()) {
      return static_cast<int>(findIt->second.setVariable(name, std::move(value)));
    }

    return -1;
  }
}
