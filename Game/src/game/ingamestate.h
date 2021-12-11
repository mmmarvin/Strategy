#ifndef INGAME_H
#define INGAME_H

#ifndef NDEBUG
#define SOL_ALL_SAFTIES_ON 1
#else
#endif // NDEBUG
#include <anax/anax.hpp>
#include "../core/gamestate/loadinggamestate.h"
#include "../core/lookup/animationset2dlookup.h"
#include "../core/gamestate.h"
#include "../core/variablemanager.h"
#include "map/map.h"
#include "unit/unitcommander.h"
#include "unit/unitselectioncreator.h"
#include "unit/unitselector.h"
#include "utility/game_command_utility.h"
#include "script/script.h"
#include "camera.h"
#include "hud.h"
#include "fogmanager.h"
#include "gameworld.h"
#include "globalresourcemanager.h"
#include "minimap.h"

namespace strat
{
  class InGameState : public gengine2d::LoadableGameState
  {
  public:
    InGameState();

    bool apply(gengine2d::vector_t<gengine2d::string_t>) override;
    bool init() override;

    gengine2d::string_t getDebugName() const override { return "INGAME_STATE"; }

    void draw(float alpha) override;
    void update(float dt) override;
    void interpolate(float, float) override {}
    void handleInput(const gengine2d::InputEvent& inputHandler) override;

    gengine2d::vector_t<gengine2d::string_t> getCommandList() const override;

  private:
    friend const gengine2d::fast_unordered_map_t<gengine2d::string_t, std::function<bool(const gengine2d::string_t&, const gengine2d::vector_t<gengine2d::string_t>&, InGameState&)>>& command_utils::getCommandList();

    void addDefaultStateVariables();

#ifndef NDEBUG
    void createDebugDatas();
#endif // NDEBUG
    void handleMouseEvent(const gengine2d::InputEvent& event,
                          const gengine2d::Vector2f& isoMapMousePosition);
    void handleHudAction(const gengine2d::InputEvent& event,
                         HUD::EHudActionState hudActionState,
                         const gengine2d::Vector2f& isoMapMousePosition);

    void setSelectedEntity(const anax::Entity& entity,
                           Player::EPlayerNumber team);
    void setSelectedEntities(const gengine2d::vector_t<anax::Entity>& entities,
                             Player::EPlayerNumber team);

    void commandUnitsToMove(const gengine2d::Vector2f& isoMapMousePosition);
    void commandUnitsToTarget(const anax::Entity& target);

    gengine2d::ResourceLookup<UnitData>     m_unitDataLookup;
    gengine2d::ResourceLookup<BuildingData> m_buildingDataLookup;
    gengine2d::Shape2DStyle                 m_buildingShowStyle;

    gengine2d::AnimationSet2DLookup         m_animationSet2DLookup;

    mar::high_resolution_clock              m_gameTimeClock;
    anax::Entity                            m_positionFlag;

    gengine2d::VariableManager              m_variableManager;

    sol::state                              m_lua;

    Camera                                  m_camera;
    GameWorld                               m_gameWorld;
    Map                                     m_map;
    GlobalResourceManager                   m_globalResourceManager;
    FogManager                              m_fogs;

    UnitSelector                            m_unitSelector;
    UnitSelectionCreator                    m_unitSelectionCreator;
    UnitCommander                           m_unitCommander;

    HUD                                     m_hud;

    const bool*                             m_varFlagShadowEnable;
    const bool*                             m_varFlagDebugShowPosition;
    const bool*                             m_varFlagDebugShowSelectionGrid;
    const bool*                             m_varFlagDebugShowBlastRadius;
  };
}

#endif // INGAME_H
