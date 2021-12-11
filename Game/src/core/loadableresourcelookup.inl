#include "loadableresourcelookup.h"

#include <fstream>

namespace gengine2d
{
  template<class T, class DeleterT>
  bool LoadableResourceLookup<T, DeleterT>::loadFromFile(const string_t& filename)
  {
    std::ifstream in(filename);
    if(in.is_open()) {
      return loadFromStream(filename, in);
    }

    return false;
  }

  template<class T, class DeleterT>
  bool LoadableResourceLookup<T, DeleterT>::loadFromStream(const string_t& filename, std::istream& stream)
  {
    return this->loadFromStreamImpl(filename, stream);
  }

  template<class T, class DeleterT>
  bool LoadableResourceLookup<T, DeleterT>::loadIndividual(const string_t& name, const string_t& filename)
  {
    std::ifstream in(filename);
    if(in.is_open()) {
      return loadIndividual(name, filename, in);
    }

    return false;
  }

  template<class T, class DeleterT>
  bool LoadableResourceLookup<T, DeleterT>::loadIndividual(const string_t& name,
                                                           const string_t& filename,
                                                           std::istream& stream)
  {
    return loadFromStreamIndividualImpl(name, filename, stream);
  }

  template<class T, class DeleterT>
  void LoadableResourceLookup<T, DeleterT>::clear()
  {
    m_resourceLookup.clear();
  }

  template<class T, class DeleterT>
  bool LoadableResourceLookup<T, DeleterT>::addResource(string_t name, T&& resource)
  {
    return m_resourceLookup.addResource(name, std::forward<T>(resource));
  }

  template<class T, class DeleterT>
  bool LoadableResourceLookup<T, DeleterT>::removeResource(string_t name)
  {
    return m_resourceLookup.removeResource(name);
  }
}
