#ifndef VARIABLE_COMMON_H
#define VARIABLE_COMMON_H

#include "../variablemanager.h"
#include "../type.h"

namespace gengine2d
{
namespace variable_common
{
  bool hasRequiredNumberOfTokens(std::size_t numberOfTokens, std::size_t required, const string_t& name);
  gengine2d::VariableManager::ESetReturn setVariable(gengine2d::VariableManager& variableManager,
                                                     const gengine2d::string_t& key,
                                                     const gengine2d::string_t& value);
}
}

#endif // VARIABLE_COMMON_H
