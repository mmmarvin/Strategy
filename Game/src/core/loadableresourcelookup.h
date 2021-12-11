#ifndef LOADABLERESOURCELOOKUP_H
#define LOADABLERESOURCELOOKUP_H

#include "resourcelookup.h"

namespace gengine2d
{
  template<class T,
           class DeleterT = detail::ResourceLookupDefaultDeleter<T>>
  class LoadableResourceLookup
  {
  public:
    LoadableResourceLookup() {}

    bool loadFromFile(const string_t& filename);
    bool loadFromStream(const string_t& filename, std::istream& stream);

    bool loadIndividual(const string_t& name, const string_t& filename);
    bool loadIndividual(const string_t& name, const string_t& filename, std::istream& stream);

    bool exists(const string_t& name) const { return m_resourceLookup.exists(name); }
    const T& getResource(const string_t& name) const { return m_resourceLookup.getResource(name); }
    const T& getResourceByIndex(std::size_t index) const { return m_resourceLookup.getResourceByIndex(index); }

  protected:
    void clear();

    bool addResource(string_t name, T&& resource);
    bool removeResource(string_t name);

  private:
    virtual bool loadFromStreamImpl(const string_t& filename, std::istream& stream) = 0;
    virtual bool loadFromStreamIndividualImpl(const string_t& name,
                                              const string_t& filename,
                                              std::istream& stream) = 0;

    ResourceLookup<T, DeleterT> m_resourceLookup;
  };
}

#include "loadableresourcelookup.inl"

#endif // LOADABLERESOURCELOOKUP_H
