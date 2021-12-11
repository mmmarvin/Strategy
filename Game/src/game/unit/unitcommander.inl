#include "unitcommander.h"

#include "../../core/utility/container_utility.h"

namespace strat
{
  template<class Func>
  bool UnitCommander::isAllEntitiesSelected(Func&& func) const
  {
    for(std::size_t i = 0, isize = m_selectedEntities.size(); i < isize; ) {
      const auto& entity = m_selectedEntities[i];

      if(entity.isValid()) {
        if(!std::forward<Func>(func)(entity)) {
          return false;
        }

        ++i;
      } else {
        isize = gengine2d::container_utils::swapAndPop(m_selectedEntities, i);
      }
    }

    if(m_selectedEntities.empty()) {
      return false;
    }

    return true;
  }

  template<class Func>
  bool UnitCommander::isAtleastOneEntitySelected(Func&& func) const
  {
    for(std::size_t i = 0, isize = m_selectedEntities.size(); i < isize;) {
      const auto& entity = m_selectedEntities[i];

      if(entity.isValid()) {
        if(std::forward<Func>(func)(entity)) {
          return true;
        }

        ++i;
      } else {
        isize = gengine2d::container_utils::swapAndPop(m_selectedEntities, i);
      }
    }

    return false;
  }

  template<class Func>
  void UnitCommander::forEachEntitiesSelected(Func&& func)
  {
#pragma message("Warning: only use this for commanding entities that are not units")
    // only use this for commanding entities that are not units
    for(std::size_t i = 0, isize = m_selectedEntities.size(); i < isize; ) {
      const auto& entity = m_selectedEntities[i];

      if(entity.isValid()) {
        std::forward<Func>(func)(entity);
        ++i;
      } else {
        isize = gengine2d::container_utils::swapAndPop(m_selectedEntities, i);
      }
    }
  }
}
