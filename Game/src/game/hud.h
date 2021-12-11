#ifndef HUD_H
#define HUD_H

#include <anax/anax.hpp>
#include "../core/resourcelookup.h"
#include "../core/type.h"
#include "data/building_data.h"
#include "data/unit_data.h"
#include "hudchat.h"
#include "hudmessage.h"
#include "minimap.h"
#include "player.h"

namespace strat
{
  class GlobalResourceManager;
  class HUD
  {
  public:
    enum class EHudActionState : unsigned char
    {
      None,
      Stop,
      Attack,
      Move,
      Build,
      ChooseBuild,
      ChooseTrainUnit,
      TrainUnit,
      Gather,
      ChoosingOneUnit,
      ChoosingMultipleUnits,
      MinimapMove
    };

    enum EHudEventResult : unsigned int
    {
      HER_NONE = 0,
      HER_KEY_PRESSED,
      HER_MOUSE_PRESSED
    };

    HUD(Map& map,
        GameWorld& gameWorld,
        Camera& camera,
        GlobalResourceManager& globalResourceManager,
        const gengine2d::Vector2f& mousePosition,
        const FogManager& fogManager,
        const gengine2d::ResourceLookup<BuildingData>& buildingDataLookup,
        const gengine2d::ResourceLookup<UnitData>& unitDataLookup);
    bool init();

    void draw(double gameTime,
              Player::EPlayerNumber team,
              const gengine2d::Vector2<std::size_t>& mapSize);
    void update();
    EHudEventResult handleEvent(const gengine2d::InputEvent& event,
                                const gengine2d::Vector2f& mousePosition,
                                Player::EPlayerNumber team,
                                GlobalResourceManager& globalResourceManager);

    void setSelectedEntity(const anax::Entity& entity,
                           Player::EPlayerNumber team,
                           GlobalResourceManager& globalResourceManager);
    void setSelectedEntities(gengine2d::vector_t<anax::Entity> entities,
                             Player::EPlayerNumber team,
                             GlobalResourceManager& globalResourceManager);
    const anax::Entity& getSelectedEntity() const { return m_selectedEntity; }
    const gengine2d::vector_t<anax::Entity>& getSelectedEntities() const { return m_selectedEntities; }

    void showMessage(const gengine2d::string_t& msg);

    void setActionState(EHudActionState actionState);
    void resetActionState();
    EHudActionState getActionState() const { return m_actionState; }

    const void* getBuildData() const { return m_buildData; }
    const gengine2d::Vector2f& getMinimapMousePosition() const { return m_minimapMousePosition; }

    bool isMenuMode() const { return m_menuMode; }

    template<class Func>
    bool forEachButton(Func&& func);

  private:
    using btn_tooltip_callback_type = std::function<void(HUD&)>;
    using btn_action_callback_type = std::function<void(HUD&)>;
    using button_info_type = std::tuple<gengine2d::string_t,
                                        const gengine2d::game_internal::Renderer::Texture*,
                                        bool,
                                        gengine2d::string_t,
                                        btn_tooltip_callback_type,
                                        btn_action_callback_type>;

    bool contains(const gengine2d::Vector2f& mousePosition) const;
    void toggleMenuMode();

    void drawResourceBar(int foodCount,
                         int goldCount,
                         int stoneCount,
                         int woodCount,
                         const gengine2d::Vector2u& renderSize,
                         const gengine2d::Vector2f& minimapStartPos);
    void drawMinimap(Player::EPlayerNumber team,
                     const gengine2d::Vector2<std::size_t>& mapSize,
                     const gengine2d::Vector2f& minimapStartPos);
    void drawExtraBar(double gameTime,
                      const gengine2d::Vector2f& minimapStartPos);
    void drawHUD(const gengine2d::Vector2u& renderSize, GlobalResourceManager& globalResourceManager);
    void drawMainMenu();
    void drawUISelectedEntityInfo();
    void drawUISelectedEntitiesInfo(GlobalResourceManager& globalResourceManager);
    void drawUISelectedEntitiesGroupInfo(GlobalResourceManager& globalResourceManager);
    void drawUIButtons();

    void addUnitButtons();
    void addWorkerButtons(Player::EPlayerNumber team,
                          GlobalResourceManager& globalResourceManager,
                          const gengine2d::vector_t<gengine2d::string_t>& buildingsBuilt);
    void addSoldierButtons(Player::EPlayerNumber,
                           GlobalResourceManager&);
    void addBuildingButtons(Player::EPlayerNumber team,
                            GlobalResourceManager& globalResourceManager);

    gengine2d::game_internal::Renderer::Texture             m_selectedEntityImage; // TODO: Remove this
    anax::Entity                                            m_selectedEntity;
    gengine2d::vector_t<anax::Entity>                       m_selectedEntities;
    gengine2d::vector_t<gengine2d::vector_t<anax::Entity>>  m_selectedEntitiesGroup;
    Player::EPlayerNumber                                   m_selectedEntitiesTeam;

    gengine2d::vector_t<button_info_type>                   m_selectedEntityButtonActions;
    gengine2d::vector_t<button_info_type>                   m_temporarySelectedEntityButtonActions;

    Minimap                                                 m_miniMap;
    HUDMessage                                              m_hudMessage;
    HUDChat                                                 m_hudChat;

    const gengine2d::game_internal::Renderer::Texture*      m_emptyButtonTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_stopButtonTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_moveButtonTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_attackButtonTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_buildButtonTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_gatherButtonTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_trainButtonTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_foodIconTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_goldIconTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_stoneIconTexture;
    const gengine2d::game_internal::Renderer::Texture*      m_woodIconTexture;

    const gengine2d::ResourceLookup<BuildingData>*          m_buildingLookup;
    const gengine2d::ResourceLookup<UnitData>*              m_unitLookup;
    const void*                                             m_buildData;
    GlobalResourceManager*                                  m_globalResourceManager;

    const gengine2d::Vector2f*                              m_mousePosition;
    gengine2d::Vector2f                                     m_minimapMousePosition;

    EHudActionState                                         m_actionState;
    bool                                                    m_menuMode;
  };
}

#include "hud.inl"

#endif // HUD_H
