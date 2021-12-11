#include "game.h"

#include "console_err_tags.h"

namespace gengine2d
{
  template<class T, class DeleterT>
  Game::LockedResourceLookup<T, DeleterT>::LockedResourceLookup(string_t lookupTag) :
    m_lookupTag(std::move(lookupTag))
  {
  }

  template<class T, class DeleterT>
  const T& Game::LockedResourceLookup<T, DeleterT>::getResource(const string_t& name) const
  {
    if(m_resourceLookup.exists(name)) {
      return m_resourceLookup.getResource(name);
    }

    auto taggedDefault = m_lookupTag + "_DEFAULT";
    assert(m_resourceLookup.exists(taggedDefault));
    GameEngine::getInstance().getConsole().printWarning(CETAG_RES_LOADER +
                                                        "Missing resource \"" +
                                                        name +
                                                        "\". Using default resource instead...");
    return m_resourceLookup.getResource(taggedDefault);
  }

  template<class StateT, class... Args>
  void Game::addState(Args&&... args)
  {
    if(!m_statesLocked) {
      m_statesCache.emplace_back(std::make_pair(EStateAction::Push,
                                                std::make_unique<StateT>(std::forward<Args>(args)...)));
      return;
    }

    DEBUG_DEV_WARNING_LOG("Cannot add a new state, because states is locked");
  }
}
