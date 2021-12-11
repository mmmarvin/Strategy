#include "hud.h"

#include <unordered_set>
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui-SFML.h>
#include "../core/collider/rectangle.h"
#include "../core/collider/collision_test.h"
#include "../core/utility/general_utility.h"
#include "../core/utility/stl_utility.h"
#include "../core/utility/time_utility.h"
#include "../core/game.h"
#include "entity/component/base_components.h"
#include "entity/component/building_components.h"
#include "entity/component/unit_components.h"
#include "player_action_acceptor.h"
#include "game_def.h"
#include "globalresourcemanager.h"

namespace strat
{
namespace
{
  void drawUISelectedEntityAvatarInfo(const anax::Entity& entity,
                                      gengine2d::game_internal::Renderer::Texture& selectedEntityImage)
  {
    gengine2d::game_internal::Renderer::ImGuiImage(selectedEntityImage,
                                                   gengine2d::Vector2f(HUD_ENTITY_IMAGE_SIZE,
                                                                       HUD_ENTITY_IMAGE_SIZE),
                                                   gengine2d::Rectf(0.f,
                                                                    0.f,
                                                                    HUD_ENTITY_IMAGE_SIZE,
                                                                    HUD_ENTITY_IMAGE_SIZE));
  }

  void drawUISelectedEntityBaseInfo(const anax::Entity& entity)
  {
    int hp = base_component_utils::getAttribute(entity,
                                                component::BaseAttributes::BA_ATTRIB_HP);
    int maxHP = base_component_utils::getAttribute(entity,
                                                   component::BaseAttributes::BA_ATTRIB_MAX_HP);

    ImGui::Text(gengine2d::string_t("HP: " +
                                    gengine2d::stl_utils::to_string(hp) +
                                    "/" +
                                    gengine2d::stl_utils::to_string(maxHP)).c_str());
  }

  void drawUISelectedEntityUnitInfo(const anax::Entity& entity)
  {
    int armor = unit_component_utils::getAttribute(entity,
                                                   component::UnitAttributes::UA_ATTRIB_ARMOR);

    ImGui::Text(gengine2d::string_t("Armor: " +
                                    gengine2d::stl_utils::to_string(armor)).c_str());
  }

  void drawUISelectedEntitySoldierInfo(const anax::Entity& entity)
  {
    int attackDamage = unit_component_utils::getSoldierAttackAttribute(entity,
                                                                        component::UnitSoldierAttackAttributes::UAA_ATTRIB_ATTACK_DAMAGE);
    int attackRange = unit_component_utils::getSoldierAttackAttribute(entity,
                                                                      component::UnitSoldierAttackAttributes::UAA_ATTRIB_ATTACK_RANGE);
    int attackSpeed = unit_component_utils::getSoldierAttackAttribute(entity,
                                                                      component::UnitSoldierAttackAttributes::UAA_ATTRIB_ATTACK_SPEED);

    ImGui::Text(gengine2d::string_t("Attack Damage: " +
                                    gengine2d::stl_utils::to_string(attackDamage)).c_str());
    ImGui::Text(gengine2d::string_t("Attack Range: " +
                                    gengine2d::stl_utils::to_string(attackRange)).c_str());
    ImGui::EndGroup();
    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::Text(gengine2d::string_t("Attack Speed: " +
                                    gengine2d::stl_utils::to_string(attackSpeed)).c_str());
  }

  void drawUISelectedEntityResourceInfo(const anax::Entity& entity,
                                        const gengine2d::game_internal::Renderer::Texture* foodIconTexture,
                                        const gengine2d::game_internal::Renderer::Texture* goldIconTexture,
                                        const gengine2d::game_internal::Renderer::Texture* stoneIconTexture,
                                        const gengine2d::game_internal::Renderer::Texture* woodIconTexture)
  {
    int resourceValue = resource_component_utils::getAttribute(entity,
                                                               component::ResourceAttributes::RA_ATTRIB_RES_VALUE_LEFT);
    int resourceValueMax = resource_component_utils::getAttribute(entity,
                                                                  component::ResourceAttributes::RA_ATTRIB_RES_VALUE_MAX);

    auto resourceType = resource_component_utils::getResourceType(entity);
    switch(resourceType) {
    case GlobalResource::GR_RES_FOOD:
      gengine2d::game_internal::Renderer::ImGuiImage(*foodIconTexture, ImVec2(16, 16));
      ImGui::SameLine();
      break;
    case GlobalResource::GR_RES_GOLD:
      gengine2d::game_internal::Renderer::ImGuiImage(*goldIconTexture, ImVec2(16, 16));
      ImGui::SameLine();
      break;
    case GlobalResource::GR_RES_STONE:
      gengine2d::game_internal::Renderer::ImGuiImage(*stoneIconTexture, ImVec2(16, 16));
      ImGui::SameLine();
      break;
    case GlobalResource::GR_RES_WOOD:
      gengine2d::game_internal::Renderer::ImGuiImage(*woodIconTexture, ImVec2(16, 16));
      ImGui::SameLine();
      break;
    default:
      break;
    }
    ImGui::Text(string_converter_utils::to_string(resourceType).c_str());
    ImGui::Text(gengine2d::string_t(gengine2d::stl_utils::to_string(resourceValue) +
                                    "/"  +
                                    gengine2d::stl_utils::to_string(resourceValueMax)).c_str());
  }

  bool allUnits(const gengine2d::vector_t<anax::Entity>& entities)
  {
    auto findIt = std::find_if(entities.begin(), entities.end(),
    [](const anax::Entity& entity) {
      return !base_component_utils::isOfType(entity,
                                             component::BaseEntityType::EEntityType::Unit);
    });

    return findIt == entities.end();
  }

  bool allOnSameTeam(const gengine2d::vector_t<anax::Entity>& entities,
                     Player::EPlayerNumber team)
  {
    auto findIt = std::find_if(entities.begin(), entities.end(),
    [team](const anax::Entity& entity) {
      return unit_component_utils::getTeam(entity) != team;
    });

    return findIt == entities.end();
  }

  bool allWorkers(const gengine2d::vector_t<anax::Entity>& entities)
  {
    auto findIt = std::find_if(entities.begin(), entities.end(),
    [](const anax::Entity& entity) {
      return !unit_component_utils::isWorker(entity);
    });

    return findIt == entities.end();
  }

  bool allSoldiers(const gengine2d::vector_t<anax::Entity>& entities)
  {
    auto findIt = std::find_if(entities.begin(), entities.end(),
    [](const anax::Entity& entity) {
      return !unit_component_utils::isSoldier(entity);
    });

    return findIt == entities.end();
  }

  bool allEntityGroupsWorkers(const gengine2d::vector_t<gengine2d::vector_t<anax::Entity>>& entityGroups)
  {
    auto findIt = std::find_if(entityGroups.begin(), entityGroups.end(),
    [](const gengine2d::vector_t<anax::Entity>& group) {
      return !allWorkers(group);
    });

    return findIt == entityGroups.end();
  }

  bool allEntityGroupsSoldiers(const gengine2d::vector_t<gengine2d::vector_t<anax::Entity>>& entityGroups)
  {
    auto findIt = std::find_if(entityGroups.begin(), entityGroups.end(),
    [](const gengine2d::vector_t<anax::Entity>& group) {
      return !allSoldiers(group);
    });

    return findIt == entityGroups.end();
  }

  gengine2d::vector_t<gengine2d::string_t> combineWorkerBuildingsBuilt(const gengine2d::vector_t<anax::Entity>& entities)
  {
    gengine2d::vector_t<gengine2d::string_t> combinedBuildingsBuilt;
    std::unordered_set<gengine2d::string_t> combinedBuildingsBuiltSet;
    for(const auto& entity : entities) {
      const auto& buildingsBuilt = unit_component_utils::getWorkerBuildingsBuilt(entity);
      for(const auto& buildingName : buildingsBuilt) {
        if(combinedBuildingsBuiltSet.find(buildingName) == combinedBuildingsBuiltSet.end()) {
          combinedBuildingsBuilt.push_back(buildingName);
          combinedBuildingsBuiltSet.emplace(buildingName);
        }
      }
    }

    return combinedBuildingsBuilt;
  }

  gengine2d::vector_t<gengine2d::string_t> combineWorkerGroupsBuildingsBuilt(const gengine2d::vector_t<gengine2d::vector_t<anax::Entity>>& entityGroups)
  {
    gengine2d::vector_t<gengine2d::string_t> combinedBuildingsBuilt;
    std::unordered_set<gengine2d::string_t> combinedBuildingsBuiltSet;

    for(const auto& group : entityGroups) {
      for(const auto& entity : group) {
        const auto& buildingsBuilt = unit_component_utils::getWorkerBuildingsBuilt(entity);
        for(const auto& buildingName : buildingsBuilt) {
          if(combinedBuildingsBuiltSet.find(buildingName) == combinedBuildingsBuiltSet.end()) {
            combinedBuildingsBuilt.push_back(buildingName);
            combinedBuildingsBuiltSet.emplace(buildingName);
          }
        }
      }
    }

    return combinedBuildingsBuilt;
  }

  gengine2d::vector_t<gengine2d::vector_t<anax::Entity>> groupEntitiesByName(const gengine2d::vector_t<anax::Entity>& entities)
  {
    std::unordered_map<gengine2d::string_t, std::size_t> groupPosition;
    gengine2d::vector_t<gengine2d::vector_t<anax::Entity>> groups;

    for(const auto& entity : entities) {
      const auto& entityName = base_component_utils::getName(entity);
      if(auto it = groupPosition.find(entityName); it != groupPosition.end()) {
        groups[it->second].push_back(entity);
      } else {
        groups.emplace_back();
        groups.back().push_back(entity);
        groupPosition.emplace(entityName, groups.size() - 1);
      }
    }

    return groups;
  }

  std::pair<gengine2d::Vector2f, std::size_t> getStartingPositionForMultipleEntities(std::size_t numberOfEntities)
  {
    numberOfEntities = gengine2d::general_utils::min(numberOfEntities,
                                                     HUD_MAX_ENTITY_MULTIPLE_IMAGES);
    std::size_t numberOfColumns = gengine2d::general_utils::fast_mod(numberOfEntities,
                                                                     HUD_MAX_ENTITY_MULTIPLE_IMAGES_PER_COLUMN) == 0 ?
                                  (numberOfEntities / HUD_MAX_ENTITY_MULTIPLE_IMAGES_PER_COLUMN) :
                                  (numberOfEntities / HUD_MAX_ENTITY_MULTIPLE_IMAGES_PER_COLUMN) + 1;
    std::size_t numberOfElementsPerRow = numberOfEntities > HUD_MAX_ENTITY_MULTIPLE_IMAGES_PER_COLUMN ?
                                         HUD_MAX_ENTITY_MULTIPLE_IMAGES_PER_COLUMN :
                                         numberOfEntities;

    auto buttonBoundsX = numberOfElementsPerRow *
                         (HUD_ENTITY_MULTIPLE_IMAGE_SIZE + (ImGui::GetStyle().ItemSpacing.x * 2.f));
    auto startingPosX = ((HUD_LOWER_WIDTH - HUD_LOWER_BUTTON_AREA_WIDTH) / 2.f) -
                        (buttonBoundsX / 2.f);
    auto startingPosY = (HUD_LOWER_HEIGHT / 2.f) - (HUD_ENTITY_MULTIPLE_IMAGE_SIZE / 2.f);
    if(numberOfColumns > 1) {
      auto buttonBoundsY = (numberOfColumns *
                           (HUD_ENTITY_MULTIPLE_IMAGE_SIZE + (ImGui::GetStyle().ItemSpacing.y * 2.f)));
      startingPosY = (HUD_LOWER_HEIGHT / 2.f) - (buttonBoundsY / 2.f);
    }

    return std::make_pair(gengine2d::Vector2f(startingPosX, startingPosY), numberOfEntities);
  }
}
  HUD::HUD(Map& map,
           GameWorld& gameWorld,
           Camera& camera,
           GlobalResourceManager& globalResourceManager,
           const gengine2d::Vector2f& mousePosition,
           const FogManager& fogManager,
           const gengine2d::ResourceLookup<BuildingData>& buildingDataLookup,
           const gengine2d::ResourceLookup<UnitData>& unitDataLookup) :
    m_miniMap(map, gameWorld, camera, fogManager),
    m_buildingLookup(&buildingDataLookup),
    m_unitLookup(&unitDataLookup),
    m_buildData(nullptr),
    m_globalResourceManager(&globalResourceManager),
    m_mousePosition(&mousePosition),
    m_actionState(EHudActionState::None),
    m_menuMode(false)
  {
  }

  bool HUD::init()
  {
    if(!m_miniMap.init(MINIMAP_SIZE, MINIMAP_SIZE)) {
      gengine2d::GameEngine::getInstance().getConsole().printError("Cannot load minimap.");
      return false;
    }

    const auto& textureLookup = gengine2d::GameEngine::getInstance().getResourceManager().getTextureLookup();
    if(!textureLookup.exists("TEXTURE_BTN_EMPT") ||
       !textureLookup.exists("TEXTURE_BTN_STOP") ||
       !textureLookup.exists("TEXTURE_BTN_MOVE") ||
       !textureLookup.exists("TEXTURE_BTN_ATTK") ||
       !textureLookup.exists("TEXTURE_BTN_BULD") ||
       !textureLookup.exists("TEXTURE_BTN_GTHR") ||
       !textureLookup.exists("TEXTURE_BTN_TRAIN") ||
       !textureLookup.exists("TEXTURE_ICO_FOOD") ||
       !textureLookup.exists("TEXTURE_ICO_GOLD") ||
       !textureLookup.exists("TEXTURE_ICO_STONE") ||
       !textureLookup.exists("TEXTURE_ICO_WOOD")) {
      return false;
    }

    m_emptyButtonTexture = &textureLookup.getResource("TEXTURE_BTN_EMPT");
    m_stopButtonTexture = &textureLookup.getResource("TEXTURE_BTN_STOP");
    m_moveButtonTexture = &textureLookup.getResource("TEXTURE_BTN_MOVE");
    m_attackButtonTexture = &textureLookup.getResource("TEXTURE_BTN_ATTK");
    m_buildButtonTexture = &textureLookup.getResource("TEXTURE_BTN_BULD");
    m_gatherButtonTexture = &textureLookup.getResource("TEXTURE_BTN_GTHR");
    m_trainButtonTexture = &textureLookup.getResource("TEXTURE_BTN_TRAIN");

    m_foodIconTexture = &textureLookup.getResource("TEXTURE_ICO_FOOD");
    m_goldIconTexture = &textureLookup.getResource("TEXTURE_ICO_GOLD");
    m_stoneIconTexture = &textureLookup.getResource("TEXTURE_ICO_STONE");
    m_woodIconTexture = &textureLookup.getResource("TEXTURE_ICO_WOOD");

    if(!m_hudMessage.init()) {
      return false;
    }

    return true;
  }

  void HUD::draw(double gameTime,
                 Player::EPlayerNumber team,
                 const gengine2d::Vector2<std::size_t>& mapSize)
  {
    const auto& renderSize = gengine2d::GameEngine::getInstance().getRenderer().getScreenSize();
    auto minimapStartPos = ImVec2((renderSize.x / 2.f) - ((MINIMAP_SIZE + 15.f) / 2.f), 0.f);

    const auto& globalResource = m_globalResourceManager->getGlobalResource();
    int foodCount = globalResource.getResourceCount(team, GlobalResource::EGlobalResouceType::GR_RES_FOOD);
    int goldCount = globalResource.getResourceCount(team, GlobalResource::EGlobalResouceType::GR_RES_GOLD);
    int stoneCount = globalResource.getResourceCount(team, GlobalResource::EGlobalResouceType::GR_RES_STONE);
    int woodCount = globalResource.getResourceCount(team, GlobalResource::EGlobalResouceType::GR_RES_WOOD);

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 1.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    drawResourceBar(foodCount, goldCount, stoneCount, woodCount, renderSize, minimapStartPos);
    drawMinimap(team, mapSize, minimapStartPos);
    drawExtraBar(gameTime, minimapStartPos);
    drawHUD(renderSize, *m_globalResourceManager);
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(1);

    m_hudMessage.draw();
    m_hudChat.draw();

    if(m_menuMode) {
      drawMainMenu();
    }
  }

  void HUD::update()
  {
    if(!m_temporarySelectedEntityButtonActions.empty()) {
      swap(m_selectedEntityButtonActions, m_temporarySelectedEntityButtonActions);
      m_temporarySelectedEntityButtonActions.clear();
    }
  }

  HUD::EHudEventResult HUD::handleEvent(const gengine2d::InputEvent& event,
                                        const gengine2d::Vector2f& mousePosition,
                                        Player::EPlayerNumber team,
                                        GlobalResourceManager& globalResourceManager)
  {
    auto type = event.type;
    if(m_menuMode || m_hudChat.isMessageMode()) {
      if(type == gengine2d::InputEvent::EEventType::ET_KEYBOARD_KEY_PRESSED) {
        if(event.keyboardEvent.key == gengine2d::InputEvent::EKeyboardKey::KK_KEY_ESC) {
          if(m_menuMode) {
            m_menuMode = false;
          } else if(m_hudChat.isMessageMode()) {
            m_hudChat.setMessageMode(false);
          }
        }
      }

      return HER_KEY_PRESSED;
    } else {
      if(type == gengine2d::InputEvent::EEventType::ET_KEYBOARD_KEY_PRESSED) {
        if(event.keyboardEvent.key == gengine2d::InputEvent::EKeyboardKey::KK_KEY_ESC) {
          if(m_actionState != HUD::EHudActionState::None) {
            setSelectedEntity(m_selectedEntity, team, globalResourceManager);
            resetActionState();
          } else {
            toggleMenuMode();
          }

          return HER_KEY_PRESSED;
        } else if(event.keyboardEvent.key == gengine2d::InputEvent::EKeyboardKey::KK_KEY_T) {
          if(!m_hudChat.isMessageMode()) {
            m_hudChat.setMessageMode(true);
          }
        }
      }

      if(type == gengine2d::InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED) {
        auto button = event.mouseEvent.button;
        if(button == gengine2d::InputEvent::EMouseButton::MB_LEFT ||
           button == gengine2d::InputEvent::EMouseButton::MB_RIGHT) {
          if(contains(mousePosition)) {
            return HER_MOUSE_PRESSED;
          }
        }
      }
    }

    return HER_NONE;
  }

  void HUD::setSelectedEntity(const anax::Entity& entity,
                              Player::EPlayerNumber team,
                              GlobalResourceManager& globalResourceManager)
  {
    m_selectedEntity = entity;
    m_selectedEntities.clear();
    m_selectedEntitiesGroup.clear();
    m_selectedEntityButtonActions.clear();
    m_temporarySelectedEntityButtonActions.clear();

    if(m_selectedEntity.isValid() &&
       !base_component_utils::isOfType(m_selectedEntity,
                                       component::BaseEntityType::EEntityType::Props) &&
       !base_component_utils::isOfType(m_selectedEntity,
                                       component::BaseEntityType::EEntityType::Resource)) {
      if(unit_component_utils::getTeam(m_selectedEntity) == team) {
        if(base_component_utils::isOfType(m_selectedEntity,
                                          component::BaseEntityType::EEntityType::Unit)) {
          addUnitButtons();

          if(unit_component_utils::isSoldier(m_selectedEntity)) {
            addSoldierButtons(team, globalResourceManager);
          }
          if(unit_component_utils::isWorker(m_selectedEntity)) {
            addWorkerButtons(team,
                             globalResourceManager,
                             unit_component_utils::getWorkerBuildingsBuilt(m_selectedEntity));
          }
        } else if(base_component_utils::isOfType(m_selectedEntity,
                                                 component::BaseEntityType::EEntityType::Building)) {
            addBuildingButtons(team, globalResourceManager);
        }
      }
    }
  }

  void HUD::setSelectedEntities(gengine2d::vector_t<anax::Entity> entities,
                                Player::EPlayerNumber team,
                                GlobalResourceManager &globalResourceManager)
  {
    // NOTE: it is assumed that the entities selected are units and that they are all
    // on the same team
    auto groups = groupEntitiesByName(entities);
    if(groups.size() > 1) {
      std::swap(groups, m_selectedEntitiesGroup);
      std::sort(m_selectedEntitiesGroup.begin(), m_selectedEntitiesGroup.end(),
      [](const gengine2d::vector_t<anax::Entity>& lhs, const gengine2d::vector_t<anax::Entity>& rhs) {
        return base_component_utils::getName(lhs.front()) < base_component_utils::getName(rhs.front());
      });
      m_selectedEntitiesTeam = team;
      m_selectedEntities.clear();

      m_selectedEntityButtonActions.clear();
      m_temporarySelectedEntityButtonActions.clear();

      addUnitButtons();
      if(allEntityGroupsWorkers(m_selectedEntitiesGroup)) {
        addWorkerButtons(team,
                         globalResourceManager,
                         combineWorkerGroupsBuildingsBuilt(m_selectedEntitiesGroup));
      }
      if(allEntityGroupsSoldiers(m_selectedEntitiesGroup)) {
        addSoldierButtons(team, globalResourceManager);
      }
    } else {
      std::swap(m_selectedEntities, entities);
      m_selectedEntitiesTeam = team;
      m_selectedEntitiesGroup.clear();

      m_selectedEntityButtonActions.clear();
      m_temporarySelectedEntityButtonActions.clear();

      addUnitButtons();
      if(allWorkers(m_selectedEntities)) {
        addWorkerButtons(team,
                         globalResourceManager,
                         combineWorkerBuildingsBuilt(m_selectedEntities));
      } else if(allSoldiers(m_selectedEntities)) {
        addSoldierButtons(team, globalResourceManager);
      }
    }
  }

  void HUD::showMessage(const gengine2d::string_t& msg)
  {
    m_hudMessage.showMessage(msg);
  }

  void HUD::setActionState(EHudActionState actionState)
  {
    m_actionState = actionState;
  }

  void HUD::resetActionState()
  {
    setActionState(EHudActionState::None);
  }

  bool HUD::contains(const gengine2d::Vector2f& mousePosition) const
  {
    const auto& renderSize = gengine2d::GameEngine::getInstance().getRenderer().getScreenSize();
    auto miniMapPos = gengine2d::Vector2f((renderSize.x / 2.f) - ((MINIMAP_SIZE + 15.f) / 2.f), 0.f);
    auto lowerHudPos = gengine2d::Vector2f((renderSize.x / 2.f) - (HUD_LOWER_WIDTH / 2.f),
                                           renderSize.y - HUD_LOWER_HEIGHT);

    gengine2d::Rectanglef miniMapHud(miniMapPos.x, miniMapPos.y, MINIMAP_SIZE + 15.f, MINIMAP_SIZE + 15.f);
    gengine2d::Rectanglef lowerHud(lowerHudPos.x, lowerHudPos.y, HUD_LOWER_WIDTH, HUD_LOWER_HEIGHT);

    if(mousePosition.y <= HUD_UPPER_HEIGHT * 3.f ||
       gengine2d::contains(miniMapHud, mousePosition) ||
       gengine2d::contains(lowerHud, mousePosition)) {
      return true;
    }

    return false;
  }

  void HUD::toggleMenuMode()
  {
    m_menuMode = !m_menuMode;
  }

  void HUD::drawResourceBar(int foodCount,
                            int goldCount,
                            int stoneCount,
                            int woodCount,
                            const gengine2d::Vector2u& renderSize,
                            const gengine2d::Vector2f& minimapStartPos)
  {
    auto foodCountStr = gengine2d::stl_utils::to_string(foodCount);
    auto goldCountStr = gengine2d::stl_utils::to_string(goldCount);
    auto stoneCountStr = gengine2d::stl_utils::to_string(stoneCount);
    auto woodCountStr = gengine2d::stl_utils::to_string(woodCount);

    ImGui::SetNextWindowPos(ImVec2(0.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(minimapStartPos.x, HUD_UPPER_HEIGHT));
    ImGui::Begin("#UI.HUD.Resources", 0, ImGuiWindowFlags_NoResize |
                                     ImGuiWindowFlags_NoTitleBar |
                                     ImGuiWindowFlags_NoScrollbar);
    if(ImGui::Button("Menu", ImVec2(HUD_MENU_BUTTON_SIZE, 0.f))) {
      m_menuMode = true;
    }
    ImGui::SameLine();
    ImGui::BeginChild("#UI.HUD.Resource.Food", ImVec2(HUD_RESOURCE_PORTION_SIZE,
                                                      HUD_RESOURCE_ICON_SIZE));

    gengine2d::game_internal::Renderer::ImGuiImage(*m_foodIconTexture, ImVec2(HUD_RESOURCE_ICON_SIZE,
                                                                              HUD_RESOURCE_ICON_SIZE));
    ImGui::SameLine();
    ImGui::Text(foodCountStr.c_str());
    ImGui::EndChild();
    if(ImGui::IsItemHovered()) {
      ImGui::SetTooltip(gengine2d::string_t("Food: " + foodCountStr).c_str());
    }
    ImGui::SameLine();
    ImGui::BeginChild("#UI.HUD.Resource.Gold", ImVec2(HUD_RESOURCE_PORTION_SIZE,
                                                      HUD_RESOURCE_ICON_SIZE));
    gengine2d::game_internal::Renderer::ImGuiImage(*m_goldIconTexture, ImVec2(HUD_RESOURCE_ICON_SIZE,
                                                                              HUD_RESOURCE_ICON_SIZE));
    ImGui::SameLine();
    ImGui::Text(goldCountStr.c_str());
    ImGui::EndChild();
    if(ImGui::IsItemHovered()) {
      ImGui::SetTooltip(gengine2d::string_t("Gold: " + goldCountStr).c_str());
    }
    ImGui::SameLine();
    ImGui::SameLine();
    ImGui::BeginChild("#UI.HUD.Resource.Stone", ImVec2(HUD_RESOURCE_PORTION_SIZE,
                                                       HUD_RESOURCE_ICON_SIZE));
    gengine2d::game_internal::Renderer::ImGuiImage(*m_stoneIconTexture, ImVec2(HUD_RESOURCE_ICON_SIZE,
                                                                                HUD_RESOURCE_ICON_SIZE));
    ImGui::SameLine();
    ImGui::Text(stoneCountStr.c_str());
    ImGui::EndChild();
    if(ImGui::IsItemHovered()) {
      ImGui::SetTooltip(gengine2d::string_t("Stone: " + stoneCountStr).c_str());
    }
    ImGui::SameLine();
    ImGui::SameLine();
    ImGui::BeginChild("#UI.HUD.Resource.Wood", ImVec2(HUD_RESOURCE_PORTION_SIZE,
                                                      HUD_RESOURCE_ICON_SIZE));
    gengine2d::game_internal::Renderer::ImGuiImage(*m_woodIconTexture, ImVec2(HUD_RESOURCE_ICON_SIZE,
                                                                               HUD_RESOURCE_ICON_SIZE));
    ImGui::SameLine();
    ImGui::Text(woodCountStr.c_str());
    ImGui::EndChild();
    if(ImGui::IsItemHovered()) {
      ImGui::SetTooltip(gengine2d::string_t("Wood: " + woodCountStr).c_str());
    }
    ImGui::SameLine();
    ImGui::End();
  }

  void HUD::drawMinimap(Player::EPlayerNumber team,
                        const gengine2d::Vector2<std::size_t>& mapSize,
                        const gengine2d::Vector2f& minimapStartPos)
  {
    m_miniMap.draw(team, mapSize);
    ImGui::SetNextWindowPos(minimapStartPos);
    ImGui::SetNextWindowSize(ImVec2(MINIMAP_SIZE + 15.f, MINIMAP_SIZE + 15.f));
    ImGui::Begin("#UI.HUD.Minimap", 0, ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoTitleBar |
                                       ImGuiWindowFlags_NoScrollbar);
    auto minimapPosition = ImGui::GetCursorScreenPos();
    gengine2d::game_internal::Renderer::ImGuiImage(m_miniMap.getTexture(),
                                                   gengine2d::Rect<gengine2d::render::position_point_2d_t>(0.f,
                                                                                                           0.f,
                                                                                                           MINIMAP_SIZE,
                                                                                                           MINIMAP_SIZE));

    if(ImGui::IsItemClicked()) {
      setActionState(EHudActionState::MinimapMove);

      m_minimapMousePosition = *m_mousePosition - gengine2d::Vector2f(minimapPosition.x, minimapPosition.y);
      m_minimapMousePosition.x = gengine2d::general_utils::clamp(m_minimapMousePosition.x, 0.f, MINIMAP_SIZE);
      m_minimapMousePosition.y = gengine2d::general_utils::clamp(m_minimapMousePosition.y, 0.f, MINIMAP_SIZE);
    }
    ImGui::End();
  }

  void HUD::drawExtraBar(double gameTime,
                         const gengine2d::Vector2f& minimapStartPos)
  {
    ImGui::SetNextWindowPos(ImVec2(minimapStartPos.x + MINIMAP_SIZE + 15.f, 0.f));
    ImGui::SetNextWindowSize(ImVec2(minimapStartPos.x, HUD_UPPER_HEIGHT));
    ImGui::Begin("#UI.HUD.Extras", 0, ImGuiWindowFlags_NoResize |
                                      ImGuiWindowFlags_NoTitleBar |
                                      ImGuiWindowFlags_NoScrollbar);
    auto [min, sec] = gengine2d::time_utils::convertSecToMinAndSec(gameTime);
    auto minStr = min < 10 ?
                  gengine2d::string_t("0") + gengine2d::stl_utils::to_string(min) :
                  gengine2d::stl_utils::to_string(min);
    auto secStr = sec < 10 ?
                  gengine2d::string_t("0") + gengine2d::stl_utils::to_string(sec) :
                  gengine2d::stl_utils::to_string(sec);
    ImGui::Text(gengine2d::string_t("Time: " +
                                    minStr +
                                    ":" +
                                    secStr).c_str());
    ImGui::End();
  }

  void HUD::drawHUD(const gengine2d::Vector2u& renderSize, GlobalResourceManager& globalResourceManager)
  {
    ImGui::SetNextWindowPos(ImVec2((renderSize.x / 2.f) - (HUD_LOWER_WIDTH / 2.f),
                                   renderSize.y - HUD_LOWER_HEIGHT));
    ImGui::SetNextWindowSize(ImVec2(HUD_LOWER_WIDTH, HUD_LOWER_HEIGHT));
    ImGui::Begin("#UI.HUD.Info", 0, ImGuiWindowFlags_NoResize |
                                    ImGuiWindowFlags_NoTitleBar |
                                    ImGuiWindowFlags_NoScrollbar);
    if(m_selectedEntitiesGroup.empty()) {
      if(m_selectedEntities.empty()) {
        if(m_selectedEntity.isValid()) {
          drawUISelectedEntityInfo();
        }
      } else {
        drawUISelectedEntitiesInfo(globalResourceManager);
      }
    } else {
      drawUISelectedEntitiesGroupInfo(globalResourceManager);
    }
    ImGui::SetCursorPosX(HUD_LOWER_WIDTH - HUD_LOWER_BUTTON_AREA_WIDTH);
    ImGui::SetCursorPosY(0);
    ImGui::BeginChild("#UI.HUD.Buttons",
                      ImVec2(HUD_LOWER_BUTTON_AREA_WIDTH, HUD_LOWER_HEIGHT),
                      true,
                      ImGuiWindowFlags_NoScrollbar);
    drawUIButtons();
    ImGui::EndChild();
    ImGui::End();
  }

  void HUD::drawMainMenu()
  {
    if(!ImGui::IsPopupOpen("##UI.HUD.GameMenu") && !ImGui::IsPopupOpen("Console")) {
      ImGui::OpenPopup("##UI.HUD.GameMenu");
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(300, 150));
    if(ImGui::BeginPopupModal("##UI.HUD.GameMenu", 0, ImGuiWindowFlags_NoTitleBar |
                                                      ImGuiWindowFlags_NoMove |
                                                      ImGuiWindowFlags_NoResize)) {
      if(ImGui::Button("Resume", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f))) {
        m_menuMode = false;
      }
      ImGui::Button("Options", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f));

      if(ImGui::Button("Exit", ImVec2(ImGui::GetContentRegionAvailWidth(), 0.f)) &&
         !ImGui::IsPopupOpen("Console")) {
        ImGui::OpenPopup("####UI.HUD.GameMenu.Exit.Continue") ;
      }

      if(ImGui::BeginPopupModal("####UI.HUD.GameMenu.Exit.Continue",
                                0,
                                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Text("You are about to leave the game. Continue?");
        if(ImGui::Button("Yes", ImVec2(100.f, 0.f))) {
          gengine2d::GameEngine::getInstance().popState();
        }
        ImGui::SameLine();
        ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 110.f);
        if(ImGui::Button("No", ImVec2(100.f, 0.f))) {
          ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
      }
      ImGui::EndPopup();
    }
    ImGui::PopStyleVar(1);
  }

  void HUD::drawUISelectedEntityInfo()
  {
    auto type = m_selectedEntity.getComponent<component::BaseEntityType>().type;

    ImGui::SetCursorPosX((HUD_LOWER_WIDTH - HUD_LOWER_BUTTON_AREA_WIDTH) / 2.f);
    ImGui::Text(base_component_utils::getName(m_selectedEntity).c_str());
    if(type == component::BaseEntityType::EEntityType::Unit ||
       type == component::BaseEntityType::EEntityType::Building ||
       type == component::BaseEntityType::EEntityType::Resource) {
      ImGui::SetCursorPosY((HUD_LOWER_HEIGHT / 2.f) - (HUD_ENTITY_IMAGE_SIZE / 2.f));
      ImGui::SetCursorPosX(25.f);
      drawUISelectedEntityAvatarInfo(m_selectedEntity, m_selectedEntityImage);
      ImGui::SameLine();
      ImGui::BeginGroup();

      if(type == component::BaseEntityType::EEntityType::Unit) {
        auto unitType = m_selectedEntity.getComponent<component::UnitType>().type;
        if(unitType == component::UnitType::EUnitType::Worker) {
          ImGui::Text("");
          drawUISelectedEntityBaseInfo(m_selectedEntity);
          drawUISelectedEntityUnitInfo(m_selectedEntity);
        } else {
          drawUISelectedEntityBaseInfo(m_selectedEntity);
          drawUISelectedEntityUnitInfo(m_selectedEntity);
          drawUISelectedEntitySoldierInfo(m_selectedEntity);
        }

      } else if(type == component::BaseEntityType::EEntityType::Building) {
        const auto& unitsTrainedQueue = building_component_utils::getUnitTrainQueue(m_selectedEntity);
        if(unitsTrainedQueue.empty()) {
          ImGui::Text("");
          drawUISelectedEntityBaseInfo(m_selectedEntity);
        } else {
          ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.f, 0.5f, 0.5f, 1.f));
          ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 1.f));
          for(std::size_t i = 0, isize = gengine2d::general_utils::min<std::size_t>(HUD_MAX_TRAIN_ICON_NUMBER, unitsTrainedQueue.size());
              i < isize;
              ++i) {
            const auto& [unitData, trainTimer] = unitsTrainedQueue[i];
            float progress = 0.f;
            if(trainTimer.is_running()) {
              progress = trainTimer.get_elapsed().as_seconds() / unitData.get().creationRequirements.creationTime;
            }
            ImGui::BeginGroup();
            gengine2d::game_internal::Renderer::ImGuiImage(*m_trainButtonTexture,
                                                           ImVec2(HUD_ENTITY_TRAIN_IMAGE_SIZE,
                                                                  HUD_ENTITY_TRAIN_IMAGE_SIZE));
            if(ImGui::IsItemHovered()) {
              ImGui::SetTooltip(unitData.get().name.c_str());
            }
            ImGui::ProgressBar(progress, ImVec2(HUD_ENTITY_TRAIN_PROGRESS_WIDTH, 0.f));
            ImGui::EndGroup();
            ImGui::SameLine();
          }
          ImGui::PopStyleColor(2);
        }
      } else if(type == component::BaseEntityType::EEntityType::Resource) {
        ImGui::Text("");
        drawUISelectedEntityResourceInfo(m_selectedEntity,
                                         m_foodIconTexture,
                                         m_goldIconTexture,
                                         m_stoneIconTexture,
                                         m_woodIconTexture);
      }

      ImGui::EndGroup();
    }
  }

  void HUD::drawUISelectedEntitiesInfo(GlobalResourceManager& globalResourceManager)
  {
    auto startingPos = getStartingPositionForMultipleEntities(m_selectedEntities.size());

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
    ImGui::SetCursorPosY(startingPos.first.y);
    std::size_t column = 0;
    for(std::size_t i = 0; i < startingPos.second; ++i) {
      const auto& entity = m_selectedEntities[i];

      if(column == 0) {
        ImGui::SetCursorPosX(startingPos.first.x);
      }
      gengine2d::game_internal::Renderer::ImGuiImageButton(m_selectedEntityImage,
                                                           ImVec2(HUD_ENTITY_MULTIPLE_IMAGE_SIZE,
                                                                  HUD_ENTITY_MULTIPLE_IMAGE_SIZE));
      if(ImGui::IsItemClicked()) {
        setSelectedEntity(entity,
                          m_selectedEntitiesTeam,
                          globalResourceManager);
        setActionState(EHudActionState::ChoosingOneUnit);
      }
      if(ImGui::IsItemHovered()) {
        ImGui::SetTooltip(base_component_utils::getName(entity).c_str());
      }

      ++column;
      if(column < HUD_MAX_ENTITY_MULTIPLE_IMAGES_PER_COLUMN) {
        ImGui::SameLine();
      } else {
        column = 0;
      }
    }
    ImGui::PopStyleColor(3);
  }

  void HUD::drawUISelectedEntitiesGroupInfo(GlobalResourceManager& globalResourceManager)
  {
    auto startingPos = getStartingPositionForMultipleEntities(m_selectedEntitiesGroup.size());

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
    ImGui::SetCursorPosY(startingPos.first.y);
    std::size_t column = 0;
    for(std::size_t i = 0; i < startingPos.second; ++i) {
      const auto& entityGroup = m_selectedEntitiesGroup[i];
      const auto& entity = entityGroup.front();
      if(column == 0) {
        ImGui::SetCursorPosX(startingPos.first.x);
      }
      ImGui::BeginGroup();
      gengine2d::game_internal::Renderer::ImGuiImageButton(m_selectedEntityImage,
                                                           ImVec2(HUD_ENTITY_MULTIPLE_IMAGE_SIZE,
                                                                  HUD_ENTITY_MULTIPLE_IMAGE_SIZE));
      ImGui::Text(gengine2d::stl_utils::to_string(entityGroup.size()).c_str());
      ImGui::EndGroup();
      if(ImGui::IsItemClicked()) {
        if(entityGroup.size() == 1) {
          setSelectedEntity(entity,
                            m_selectedEntitiesTeam,
                            globalResourceManager);
          setActionState(EHudActionState::ChoosingOneUnit);
        } else {
          setSelectedEntities(entityGroup,
                              m_selectedEntitiesTeam,
                              globalResourceManager);
          setActionState(EHudActionState::ChoosingMultipleUnits);
        }
        break;
      }
      if(ImGui::IsItemHovered()) {
        ImGui::SetTooltip(base_component_utils::getName(entity).c_str());
      }

      ++column;
      if(column < HUD_MAX_ENTITY_MULTIPLE_IMAGES_PER_COLUMN) {
        ImGui::SameLine();
      } else {
        column = 0;
      }
    }

    ImGui::PopStyleColor(3);
  }

  void HUD::drawUIButtons()
  {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.1f, 0.1f, 0.1f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 1.f));
    if(m_actionState != EHudActionState::None &&
       m_actionState != EHudActionState::Stop &&
       m_actionState != EHudActionState::ChooseBuild &&
       m_actionState != EHudActionState::ChooseTrainUnit) {
      ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
      int column = 0;
      for(std::size_t i = 0; i < HUD_BUTTON_MAX_BUTTONS; ++i) {
        gengine2d::game_internal::Renderer::ImGuiImageButton(*m_emptyButtonTexture, ImVec2(HUD_BUTTON_SIZE,
                                                                                           HUD_BUTTON_SIZE));
        ++column;
        if(column < HUD_BUTTON_MAX_BUTTONS_PER_COLUMN) {
          ImGui::SameLine();
        } else {
          column = 0;
        }
      }
      ImGui::PopItemFlag();
    } else {
      auto numberOfButtons = m_selectedEntityButtonActions.size();
      int column = 0;
      for(const auto& [btnAction, btnTexture, btnEnabled, btnToolTip, btnTooltipCallBack, btnCallBack] : m_selectedEntityButtonActions) {
        if(btnEnabled) {
          gengine2d::game_internal::Renderer::ImGuiImageButton(*btnTexture, ImVec2(HUD_BUTTON_SIZE,
                                                                                   HUD_BUTTON_SIZE));
          if(ImGui::IsItemClicked()) {
            btnCallBack(*this) ;
          }
          if(ImGui::IsItemHovered()) {
            if(btnTooltipCallBack) {
              ImGui::BeginTooltip();
              ImGui::Text(btnToolTip.c_str());
              btnTooltipCallBack(*this);
              ImGui::EndTooltip();
            } else {
              ImGui::SetTooltip(btnToolTip.c_str());
            }
          }
        } else {
          ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
          gengine2d::game_internal::Renderer::ImGuiImageButton(*btnTexture, ImVec2(HUD_BUTTON_SIZE,
                                                                                   HUD_BUTTON_SIZE));
          if(ImGui::IsItemHovered()) {
            ImGui::SetTooltip(btnToolTip.c_str());
          }
          ImGui::PopItemFlag();
        }
        ++column;
        if(column < HUD_BUTTON_MAX_BUTTONS_PER_COLUMN) {
          ImGui::SameLine();
        } else {
          column = 0;
        }
      }

      if(numberOfButtons < HUD_BUTTON_MAX_BUTTONS) {
        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        for(std::size_t i = 0, isize = HUD_BUTTON_MAX_BUTTONS - numberOfButtons; i < isize; ++i) {
          gengine2d::game_internal::Renderer::ImGuiImageButton(*m_emptyButtonTexture, ImVec2(HUD_BUTTON_SIZE,
                                                                                             HUD_BUTTON_SIZE));
          ++column;
          if(column < HUD_BUTTON_MAX_BUTTONS_PER_COLUMN) {
            ImGui::SameLine();
          } else {
            column = 0;
          }
        }
        ImGui::PopItemFlag();
      }
    }
    ImGui::PopStyleColor(3);
  }

  void HUD::addUnitButtons()
  {
    m_selectedEntityButtonActions.push_back(button_info_type("actionStop",
                                                             m_stopButtonTexture,
                                                             true,
                                                             "Stop [S]",
                                                             btn_tooltip_callback_type(),
    [](HUD& hud) {
      hud.setActionState(EHudActionState::Stop);
    }));

    m_selectedEntityButtonActions.push_back(button_info_type("actionMove",
                                                             m_moveButtonTexture,
                                                             true,
                                                             "Move [M]",
                                                             btn_tooltip_callback_type(),
    [](HUD& hud) {
      hud.setActionState(EHudActionState::Move);
    }));
  }

  void HUD::addWorkerButtons(Player::EPlayerNumber team,
                             GlobalResourceManager& globalResourceManager,
                             const gengine2d::vector_t<gengine2d::string_t>& buildingsBuilt)
  {
    m_selectedEntityButtonActions.push_back(button_info_type("actionGather",
                                                             m_gatherButtonTexture,
                                                             true,
                                                             "Gather Resource [G]",
                                                             btn_tooltip_callback_type(),
    [](HUD& hud) {
      hud.setActionState(EHudActionState::Gather);
    }));
    m_selectedEntityButtonActions.push_back(button_info_type("actionBuild",
                                                             m_buildButtonTexture,
                                                             true,
                                                             "Build [B]",
                                                             btn_tooltip_callback_type(),
    [team, &globalResourceManager, buildingsBuilt](HUD& hud) {
      hud.setActionState(EHudActionState::ChooseBuild);

      for(std::size_t i = 0, isize = gengine2d::general_utils::min<std::size_t>(HUD_MAX_NUMBER_OF_BUTTONS, buildingsBuilt.size());
          i < isize;
          ++i) {
        const auto& buildingName = buildingsBuilt[i];
        if(hud.m_buildingLookup->exists(buildingName)) {
          const auto& buildingData = hud.m_buildingLookup->getResource(buildingName);
          hud.m_temporarySelectedEntityButtonActions.push_back(std::make_tuple("",
                                                               hud.m_trainButtonTexture,
                                                               true,
                                                               "Build " + buildingData.name,
                                                               btn_tooltip_callback_type(),
          [team, &globalResourceManager, &buildingData](HUD& hud) {
            auto buildResult = action_acceptor::canBuild(buildingData,
                                                         team,
                                                         globalResourceManager);
            if(buildResult == GlobalResourceManager::RMR_SUCCESS) {
              hud.m_buildData = static_cast<const void*>(&buildingData);
              hud.setActionState(EHudActionState::Build);
            } else {
              auto buildIntRes = static_cast<int>(buildResult) - 1;
              hud.showMessage("Not enough " + string_converter_utils::to_string(static_cast<GlobalResource::EGlobalResouceType>(buildIntRes)));
            }
          }));
        }
      }
    }));
  }

  void HUD::addSoldierButtons(Player::EPlayerNumber,
                              GlobalResourceManager&)
  {
    m_selectedEntityButtonActions.push_back(button_info_type("actionAttack",
                                                             m_attackButtonTexture,
                                                             true,
                                                             "Attack [A]",
                                                             btn_tooltip_callback_type(),
    [](HUD& hud) {
      hud.setActionState(EHudActionState::Attack);
    }));
  }

  void HUD::addBuildingButtons(Player::EPlayerNumber team,
                               GlobalResourceManager& globalResourceManager)
  {
    if(!building_component_utils::getUnitsTrained(m_selectedEntity).empty()) {
      m_selectedEntityButtonActions.push_back(button_info_type("actionTrainUnit",
                                                               m_trainButtonTexture,
                                                               true,
                                                               "Train Units",
                                                               btn_tooltip_callback_type(),
      [&globalResourceManager, team](HUD& hud) {
        hud.setActionState(EHudActionState::ChooseTrainUnit);

        const auto& unitsTrained = building_component_utils::getUnitsTrained(hud.m_selectedEntity);
        for(std::size_t i = 0, isize = gengine2d::general_utils::min<std::size_t>(HUD_MAX_NUMBER_OF_BUTTONS, unitsTrained.size());
            i < isize;
            ++i) {
          const auto& unitName = unitsTrained[i];
          if(hud.m_unitLookup->exists(unitName)) {
            const auto& unitData = hud.m_unitLookup->getResource(unitName);
DISABLE_WARNING_BEGIN
DISABLE_WARNING("-Wshadow")
            hud.m_temporarySelectedEntityButtonActions.push_back(std::make_tuple("",
                                                                 hud.m_trainButtonTexture,
                                                                 true,
                                                                 "Train " + unitData.name,
            [&unitData](HUD& hud) {
              if(unitData.creationRequirements.hasGoldRequirements()) {
                gengine2d::game_internal::Renderer::ImGuiImage(*hud.m_goldIconTexture, ImVec2(16, 16));
                ImGui::SameLine();
                ImGui::Text(gengine2d::stl_utils::to_string(unitData.creationRequirements.requirements[GlobalResource::GR_RES_GOLD]).c_str());
              }
              ImGui::Spacing();

              if(unitData.creationRequirements.hasRequriements()) {
                const auto& requirementList = unitData.creationRequirements.requirements;

                ImGui::Text("Requires:");
                if(requirementList[GlobalResource::GR_RES_FOOD] > 0) {
                  gengine2d::game_internal::Renderer::ImGuiImage(*hud.m_foodIconTexture, ImVec2(16, 16));
                  ImGui::SameLine();
                  ImGui::Text(gengine2d::stl_utils::to_string(requirementList[GlobalResource::GR_RES_FOOD]).c_str());
                }
                if(requirementList[GlobalResource::GR_RES_STONE] > 0) {
                  gengine2d::game_internal::Renderer::ImGuiImage(*hud.m_stoneIconTexture, ImVec2(16, 16));
                  ImGui::SameLine();
                  ImGui::Text(gengine2d::stl_utils::to_string(requirementList[GlobalResource::GR_RES_STONE]).c_str());
                }
                if(requirementList[GlobalResource::GR_RES_WOOD] > 0) {
                  gengine2d::game_internal::Renderer::ImGuiImage(*hud.m_woodIconTexture, ImVec2(16, 16));
                  ImGui::SameLine();
                  ImGui::Text(gengine2d::stl_utils::to_string(requirementList[GlobalResource::GR_RES_WOOD]).c_str());
                }
              }
              ImGui::Spacing();
              ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + 200.f);
              ImGui::Text(unitData.description.c_str(), 200.f);
            },
            [&globalResourceManager, team, &unitData](HUD& hud) {
              auto trainResult = action_acceptor::canTrain(unitData, team, globalResourceManager);
              if(trainResult  == GlobalResourceManager::RMR_SUCCESS) {
                action_acceptor::train(unitData, hud.m_selectedEntity);
              } else {
                auto trainIntRes = static_cast<int>(trainResult ) - 1;
                hud.showMessage("Not enough " + string_converter_utils::to_string(static_cast<GlobalResource::EGlobalResouceType>(trainIntRes)));
              }
            }));
DISABLE_WARNING_END
          }
        }
      }));
    }
  }
}
