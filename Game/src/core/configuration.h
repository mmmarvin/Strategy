#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "classconsolewriter.h"
#include "variablemanager.h"
#include "type.h"

namespace gengine2d
{
  class Configuration : ClassConsoleWriter
  {
  public:
    Configuration();

    bool loadFromFile(const string_t& filename);
    bool loadFromStream(std::istream& stream);

    bool addVariable(const string_t& section,
                     const string_t& name,
                     const string_t& value,
                     EVariableType type);
    const bool* getBool(const string_t& section,
                        const string_t& name) const;
    const std::int64_t* getInteger(const string_t& section,
                                   const string_t& name) const;
    const double* getFloat(const string_t& section,
                           const string_t& name) const;
    const string_t* getString(const string_t& section,
                              const string_t& name) const;

  private:
    int setVariable(const string_t& section, const string_t& name, string_t value);

    fast_unordered_map_t<string_t, VariableManager> m_variableSections;
    VariableManager                                 m_universalVariable;
  };
}

#endif // CONFIGURATION_H
