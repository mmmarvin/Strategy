#ifndef UNITCOMMANDER_H
#define UNITCOMMANDER_H

#include <anax/anax.hpp>
#include "../../core/vector2.h"
#include "../../core/type.h"
#include "../data/building_data.h"
#include "../entity/component/unit_components.h"

namespace strat
{
  class EntityManager;

  // TODO: Add team number so that we can determine which entities are our own
  class UnitCommander
  {
  public:
    UnitCommander(EntityManager& entityManager);

    void setSelectedEntity(const anax::Entity& entity);
    void setSelectedEntities(gengine2d::vector_t<anax::Entity> entities);
    void addToSelectedEntities(const anax::Entity& entity);

    void resetAndClearSelectedEntities();

    void commandToMoveSelectedEntities(const gengine2d::Vector2f& isoPosition);
    void commandToMoveSelectedEntities(const anax::Entity& target);
    void commandToBuildSelectedEntities(const BuildingData& buildingData,
                                        const gengine2d::Vector2f& isoPosition);
    void commandToIdleSelectedEntities();
    template<class Func> void forEachEntitiesSelected(Func&& func);

    bool isEmptySelection() const { return m_selectedEntities.empty(); }
    template<class Func> bool isAllEntitiesSelected(Func&& func) const;
    template<class Func> bool isAtleastOneEntitySelected(Func&& func) const;
    bool isAllSelectedEntitiesUnits() const;
    std::size_t numberOfSelectedEntities() const { return m_selectedEntities.size(); }

  private:
    void resetSelectedEntities();

    template<class Func>
    void commandSelectedEntities(component::UnitState::EUnitState state,
                                 const anax::Entity& target,
                                 const gengine2d::Vector2f& position,
                                 Func&& func);
    void commandSelectedEntities(component::UnitState::EUnitState state);


    EntityManager*                            m_entityManager;
    mutable gengine2d::vector_t<anax::Entity> m_selectedEntities;
  };
}

#include "unitcommander.inl"

#endif // UNITCOMMANDER_H
