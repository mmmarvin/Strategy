#include "ingamestate.h"

#include <cassert>
#include <thread>
#include <imgui.h>
#include <imgui-SFML.h>
#include "../core/debug/assert_result.h"
#include "../core/debug/debug_log.h"
#include "../core/game.h"
#include "../core/utility/stl_utility.h"
#include "../core/utility/vector_utility.h"
#include "../core/common/variable_common.h"
#include "common/position_common.h"
#include "entity/component/flag_components.h"
#include "entity/component/unit_components.h"
#include "entity/component/resource_components.h"
#include "game_def.h"
#include "player_action_acceptor.h"

namespace strat
{
namespace
{
  void removeNonUnit(gengine2d::vector_t<anax::Entity>& entities)
  {
    for(std::size_t i = 0, isize = entities.size(); i < isize;) {
      const auto& entity = entities[i];

      if(!base_component_utils::isOfType(entity,
                                         component::BaseEntityType::EEntityType::Unit)) {
        isize = gengine2d::container_utils::swapAndPop(entities, i);
      } else {
        ++i;
      }
    }
  }
}

#ifndef NDEBUG
  static gengine2d::vector_t<std::pair<bool, gengine2d::string_t>> TODO_LIST =
  {
    { true, "Optimize update's drop from 20ms to 300+ms" },
    { true, "Move to Isometric" },
    { true, "Make camera work on isometric world" },
    { true, "Change console to ImGui" },
    { true, "Add autocomplete to console" },
    { false, "Add additional namespace to utilities "},
    { true, "Create projectile attacks" },
    { true, "Create UI" },
    { true, "Create minimap" },
    { true, "Create Menu" },
    { true, "Create Chat Messages" },
    { false, "Start LUA integration" },
    { false, "Create global attack events, that can be modified in lua" },
    { false, "Create global death events, that can be modified in lua" },
    { false, "Create building \"research\" in lua" },
    { false, "Create weather" }
  };
#endif // NDEBUG
  InGameState::InGameState() :
    LoadableGameState({ &m_camera.getView() }, {
      std::make_tuple("actionStop", false, gengine2d::InputEvent::EKeyboardKey::KK_KEY_S, [](GameState*) {}),
      std::make_tuple("actionAttack", false, gengine2d::InputEvent::EKeyboardKey::KK_KEY_A, [](GameState*) {}),
      std::make_tuple("actionMove", false, gengine2d::InputEvent::EKeyboardKey::KK_KEY_M, [](GameState*) {}),
      std::make_tuple("actionBuild", false, gengine2d::InputEvent::EKeyboardKey::KK_KEY_B, [](GameState*) {}),
      std::make_tuple("actionGather", false, gengine2d::InputEvent::EKeyboardKey::KK_KEY_G, [](GameState*) {})
    }),
    m_animationSet2DLookup(GameState::getTextureLookup()),
    m_gameWorld(m_map, m_fogs, m_globalResourceManager.getGlobalResource()),
    m_unitSelector(m_gameWorld.getEntityManager().getSpatialPartition()),
    m_unitSelectionCreator(GameState::getInputState()),
    m_unitCommander(m_gameWorld.getEntityManager()),
    m_hud(m_map,
          m_gameWorld,
          m_camera,
          m_globalResourceManager,
          this->getInputState().getMousePosition(0),
          m_fogs,
          m_buildingDataLookup,
          m_unitDataLookup)
  {
  }

  bool InGameState::apply(gengine2d::vector_t<gengine2d::string_t> tokens)
  {
    auto& console = gengine2d::GameEngine::getInstance().getConsole();

    auto numberOfTokens = tokens.size();
    if(numberOfTokens > 0) {
      auto key = tokens[0];
      const auto& commandList = command_utils::getCommandList();
      if(auto findIt = commandList.find(key); findIt != commandList.end()) {
        return findIt->second(key, tokens, *this);
      } else {
        if(m_variableManager.exists(key)) {
          if(numberOfTokens == 1) {
            console.printLine("\"" + tokens[0] + "\" = \"" + m_variableManager.getValueString(key) + "\"");
            return true;
          } else if(numberOfTokens == 2) {
            auto setResult = gengine2d::variable_common::setVariable(m_variableManager, key, tokens[1]);
            if(setResult == gengine2d::VariableManager::ESetReturn::Success) {
              return true;
            }
          } else {
            console.printError("Invalid number of arguments for variable command");
            return true;
          }
        }
      }
    }

    return false;
  }

  bool InGameState::init()
  {
    auto& console = gengine2d::GameEngine::getInstance().getConsole();
    addDefaultStateVariables();
    gengine2d::game_utils::loadVariables("ingame.cfg", m_variableManager);

    // Load button textures
    if(!this->addTexture("TEXTURE_BTN_EMPT", "res/button_empty.png") ||
       !this->addTexture("TEXTURE_BTN_STOP", "res/button_stop.png") ||
       !this->addTexture("TEXTURE_BTN_MOVE", "res/button_move.png") ||
       !this->addTexture("TEXTURE_BTN_ATTK", "res/button_attack.png") ||
       !this->addTexture("TEXTURE_BTN_BULD", "res/button_build.png") ||
       !this->addTexture("TEXTURE_BTN_GTHR", "res/button_gather.png") ||
       !this->addTexture("TEXTURE_BTN_TRAIN", "res/button_train.png") ||
       !this->addTexture("TEXTURE_ICO_FOOD", "res/icon_food.png") ||
       !this->addTexture("TEXTURE_ICO_GOLD", "res/icon_gold.png") ||
       !this->addTexture("TEXTURE_ICO_STONE", "res/icon_stone.png") ||
       !this->addTexture("TEXTURE_ICO_WOOD", "res/icon_wood.png")) {
      console.printError("Cannot load ui button images");
      return false;
    }
    this->setLoadPercentage(10);

    if(!m_hud.init()) {
      console.printError("Cannot load UI");
      return false;
    }
    this->setLoadPercentage(20);

    if(!m_gameWorld.init()) {
      return false;
    }
    this->setLoadPercentage(40);

    m_positionFlag = m_gameWorld.getEntityManager().createFlagTimed(gengine2d::Vector2f(0.f, 0.f));
    m_camera.setSize(gengine2d::Vector2f(CAMERA_SIZE_X, CAMERA_SIZE_Y));

#ifndef NDEBUG
    createDebugDatas();

    if(!m_map.loadFromFile("maps/map001.smap", MINIMAP_SIZE, MINIMAP_SIZE)) {
      return false;
    }
    this->setLoadPercentage(80);

    if(!m_fogs.init(m_map.getSize().x, m_map.getSize().y)) {
      return false;
    }

    const auto& soldierData = m_unitDataLookup.getResource("UNIT_SOLDIER");
    const auto& workerData = m_unitDataLookup.getResource("UNIT_WORKER");
    const auto& castleData = m_buildingDataLookup.getResource("BUILDING_CASTLE");

    // TODO: Remove this
    auto entity = m_gameWorld.getEntityManager().createUnit(soldierData,
                                                            Player::EPlayerNumber::PN_PLAYER1,
                                                            gengine2d::Vector2f(500, 500),
                                                            anax::Entity());

//    for(int i = 0; i < 50; ++i) {
      m_gameWorld.getEntityManager().createUnit(workerData,
                                                Player::EPlayerNumber::PN_PLAYER1,
                                                gengine2d::Vector2f(0, 250),
                                                entity);
      m_gameWorld.getEntityManager().createBuilding(castleData,
                                                    Player::EPlayerNumber::PN_PLAYER1,
                                                    gengine2d::Vector2f(100, 150),
                                                    true);

//      m_gameWorld.getEntityManager().createUnit(Player::EPlayerNumber::PN_PLAYER1,
//                                                gengine2d::Vector2f(250, 350),
//                                                component::UnitType::Worker,
//                                                entity);
//    }
    m_gameWorld.getEntityManager().createResource(gengine2d::Vector2f(700, 250),
                                                  GlobalResource::GR_RES_FOOD);

    for(int i = 0; i < 10; ++i) {
    m_gameWorld.getEntityManager().createUnit(workerData,
                                              Player::EPlayerNumber::PN_PLAYER2,
                                              gengine2d::Vector2f(230, 550),
                                              entity);
    }
    m_gameWorld.getEntityManager().createBuilding(castleData,
                                                  Player::EPlayerNumber::PN_PLAYER2,
                                                  gengine2d::Vector2f(300, 550),
                                                  true);
#endif // NDEBUG
    // END TODO

    this->setLoadPercentage(100);
    return true;
  }

  void InGameState::draw(float alpha)
  {
    auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto& inputState = this->getInputState();

    renderer.set2DCamera(m_camera.getView());
    // TODO: Change lalt to user specified
    m_map.draw();
    m_fogs.draw(Player::EPlayerNumber::PN_PLAYER1);
    m_unitSelector.draw(*m_varFlagDebugShowSelectionGrid);
    m_gameWorld.draw(alpha,
                     *m_varFlagShadowEnable,
                     inputState.isKeyboardKeyDown(gengine2d::InputEvent::EKeyboardKey::KK_KEY_LALT),
                     *m_varFlagDebugShowPosition,
                     *m_varFlagDebugShowBlastRadius);

    m_unitSelectionCreator.draw(*m_varFlagDebugShowPosition);
    if(m_hud.getActionState() == HUD::EHudActionState::Build) {
      auto isoMouseMapPosition = this->getInputState().getMousePosition(1);
      auto cartMouseMapPosition = gengine2d::vector_utils::convertIsoToCartPoint(isoMouseMapPosition);

      if(auto tileInfo = m_map.getTileInfo(cartMouseMapPosition.x / TILE_SIZE,
                                           cartMouseMapPosition.y / TILE_SIZE); tileInfo && tileInfo->walkable) {
        m_buildingShowStyle.fillColor = gengine2d::Color(0, 255, 0, 128);
      } else {
        m_buildingShowStyle.fillColor = gengine2d::Color(255, 0, 0, 128);
      }

      renderer.draw2DRect(gengine2d::Rect<gengine2d::render::position_point_2d_t>(isoMouseMapPosition.x - 32,
                                                                                  isoMouseMapPosition.y - 32,
                                                                                  32,
                                                                                  32),
                          m_buildingShowStyle);
    }

    renderer.reset2DCamera();
    m_hud.draw(m_gameTimeClock.get_elapsed().as_seconds(), Player::EPlayerNumber::PN_PLAYER1, m_map.getSize());
  }

  void InGameState::update(float dt)
  {
    const auto& inputState = this->getInputState();
    m_gameWorld.update(dt);
    m_hud.update();

    if(m_hud.getActionState() == HUD::EHudActionState::Stop) {
      m_unitCommander.commandToIdleSelectedEntities();
      m_hud.resetActionState();
    }

    if(!gengine2d::GameEngine::getInstance().isOnConsoleMode() &&
       !m_hud.isMenuMode()) {
      auto [mapWidth, mapHeight] = m_map.getSize();
      m_camera.update(dt, mapWidth, mapHeight, inputState.getMousePosition(0));

      if(inputState.isKeyboardKeyDown(gengine2d::InputEvent::EKeyboardKey::KK_KEY_LEFT)) {
        m_camera.moveLeft(dt, mapWidth, mapHeight);
      } else if(inputState.isKeyboardKeyDown(gengine2d::InputEvent::EKeyboardKey::KK_KEY_RIGHT)) {
        m_camera.moveRight(dt, mapWidth, mapHeight);
      }

      if(inputState.isKeyboardKeyDown(gengine2d::InputEvent::EKeyboardKey::KK_KEY_UP)) {
        m_camera.moveUp(dt, mapWidth, mapHeight);
      } else if(inputState.isKeyboardKeyDown(gengine2d::InputEvent::EKeyboardKey::KK_KEY_DOWN)) {
        m_camera.moveDown(dt, mapWidth, mapHeight);
      }
    }
  }

  void InGameState::handleInput(const gengine2d::InputEvent& event)
  {
    GameState::handleInput(event);

    const auto& inputState = this->getInputState();
    const auto& isoMapMousePosition = inputState.getMousePosition(1);
    // TODO: Change team
    auto hudResult = m_hud.handleEvent(event,
                                       inputState.getMousePosition(0),
                                       Player::EPlayerNumber::PN_PLAYER1,
                                       m_globalResourceManager);
    switch(hudResult) {
    case HUD::HER_KEY_PRESSED:
      return;
    case HUD::HER_MOUSE_PRESSED:
      if(!m_unitSelectionCreator.selectionStarted()) {
        return;
      }
      break;
    default:
      break;
    }

    auto type = event.type;
    if(type == gengine2d::InputEvent::EEventType::ET_MOUSE_WHEEL_SCROLLED) {
      auto wheelDelta = event.mouseEvent.wheelDirection;
      if(wheelDelta == gengine2d::InputEvent::EWheelDirection::WD_DOWN) {
        m_camera.zoomOut();
      } else if(wheelDelta == gengine2d::InputEvent::EWheelDirection::WD_UP) {
        m_camera.zoomIn();
      }
    } else if(type == gengine2d::InputEvent::EEventType::ET_KEYBOARD_KEY_PRESSED) {
      auto key = event.keyboardEvent.key;
      if(this->getControlMap().forEachKeyMapped(
        [key, this](const gengine2d::string_t& controlName,
                    bool isMouse,
                    long int keyMapped,
                    const std::function<void(GameState*)>&) {
          if(!isMouse && key == keyMapped) {
            if(m_hud.forEachButton(
              [&controlName, this](const gengine2d::string_t& btnAction,
                                   const gengine2d::game_internal::Renderer::Texture*,
                                   bool,
                                   const gengine2d::string_t&,
                                   const std::function<void(HUD&)>&,
                                   const std::function<void(HUD&)>& btnCallback) {
              if(btnAction == controlName) {
                btnCallback(m_hud);
                return true;
              }

              return false;
            })) {
              return true;
            }
          }

          return false;
        })) {
        return;
      }
    } else if(type == gengine2d::InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED) {
      auto button = event.mouseEvent.button;
      if(button != gengine2d::InputEvent::EMouseButton::MB_LEFT &&
         button != gengine2d::InputEvent::EMouseButton::MB_RIGHT) {
        if(this->getControlMap().forEachKeyMapped(
          [button, this](const gengine2d::string_t& controlName,
                         bool isMouse,
                         long int buttonMapped,
                         const std::function<void(GameState*)>&) {
            if(isMouse && button == buttonMapped) {
              if(m_hud.forEachButton(
                [&controlName, this](const gengine2d::string_t& btnAction,
                                     const gengine2d::game_internal::Renderer::Texture*,
                                     bool,
                                     const gengine2d::string_t&,
                                     const std::function<void(HUD&)>&,
                                     const std::function<void(HUD&)>& btnCallback) {
                if(btnAction == controlName) {
                  btnCallback(m_hud);
                  return true;
                }

                return false;
              })) {
                return true;
              }
            }

            return false;
          })) {
          return;
        }
      }
    }

    handleMouseEvent(event, isoMapMousePosition);
  }

  gengine2d::vector_t<gengine2d::string_t> InGameState::getCommandList() const
  {
    gengine2d::vector_t<gengine2d::string_t> ret = m_variableManager.getVariableList();

    return ret;
  }

  void InGameState::addDefaultStateVariables()
  {
    assert_result(m_variableManager.addVariable("client.render.shadow_enable",
                                                "true",
                                                gengine2d::EVariableType::Boolean));
    assert_result(m_variableManager.addVariable("client.debug.show_position",
                                                "true",
                                                gengine2d::EVariableType::Boolean));
    assert_result(m_variableManager.addVariable("client.debug.show_selection_grids",
                                                "true",
                                                gengine2d::EVariableType::Boolean));
    assert_result(m_variableManager.addVariable("client.debug.show_blast_radius",
                                                "true",
                                                gengine2d::EVariableType::Boolean));

    m_varFlagShadowEnable = m_variableManager.getBool("client.render.shadow_enable");
    m_varFlagDebugShowPosition = m_variableManager.getBool("client.debug.show_position");
    m_varFlagDebugShowSelectionGrid = m_variableManager.getBool("client.debug.show_selection_grids");
    m_varFlagDebugShowBlastRadius = m_variableManager.getBool("client.debug.show_blast_radius");
  }
#ifndef NDEBUG
  void InGameState::createDebugDatas()
  {
    UnitData unitData;
    unitData.name = "Soldier";
    unitData.colliderOffsetX = 0.f;
    unitData.colliderOffsetY = 0.f;
    unitData.colliderRadius = 1.f;
    unitData.health = 100.f;
    unitData.creationRequirements.creationTime = 100.f;
    unitData.moveSpeed = 10.f;
    unitData.armor = 0.f;
    unitData.unitType = UnitData::UT_SOLDIER;
    unitData.attackType = UnitData::EAttackType::AT_TYPE_PROJECTILE;
    unitData.attackDamage = 30.f;
    unitData.attackRange = 10.f;
    unitData.attackSpeed = 5.f;
    unitData.attackAnimationSpeed = 0.f;
    unitData.creationRequirements.requirements[GlobalResource::GR_RES_GOLD] = 100;

    m_unitDataLookup.addResource("UNIT_SOLDIER", UnitData(unitData));

    unitData.name = "Worker";
    unitData.creationRequirements.creationTime = 50.f;
    unitData.unitType = UnitData::UT_WORKER;
    unitData.attackType = UnitData::EAttackType::AT_TYPE_NONE;
    unitData.attackDamage = 0.f;
    unitData.attackRange = 0.f;
    unitData.attackSpeed = 0.f;
    unitData.attackAnimationSpeed = 0.f;
    unitData.buildingsBuilt =
    {
      "BUILDING_CASTLE"
    };
    unitData.creationRequirements.requirements[GlobalResource::GR_RES_WOOD] = 10;
    unitData.creationRequirements.requirements[GlobalResource::GR_RES_GOLD] = 50;
    unitData.description = "Create worker units that can build and gather resources";

    m_unitDataLookup.addResource("UNIT_WORKER", UnitData(unitData));

    BuildingData buildingData;
    buildingData.name = "Castle";
    buildingData.colliderOffsetX = 0.f;
    buildingData.colliderOffsetY = 0.f;
    buildingData.colliderRadius = 1.f;
    buildingData.type = BuildingData::EBuildingType::BT_TYPE_CASTLE;
    buildingData.health = 100.f;
    buildingData.unitsTrained =
    {
      "UNIT_WORKER",
      "UNIT_SOLDIER"
    };
    buildingData.creationRequirements.requirements[GlobalResource::GR_RES_GOLD] = 50;
    buildingData.attackType = AttackData::EAttackType::AT_TYPE_PROJECTILE;
    buildingData.attackDamage = 10.f;
    buildingData.attackDamageDropoff = 5.f;
    buildingData.attackDamageMin = 2.5f;
    buildingData.attackDamageRadius = 1.f;
    buildingData.attackRange = 48.f;
    buildingData.attackSpeed = 1.f;

    m_buildingDataLookup.addResource("BUILDING_CASTLE", BuildingData(buildingData));
  }
#endif // NDEBUG
  void InGameState::handleMouseEvent(const gengine2d::InputEvent& event,
                                     const gengine2d::Vector2f& isoMapMousePosition)
  {
    auto type = event.type;
    auto hudActionState = m_hud.getActionState();
    if(hudActionState != HUD::EHudActionState::None &&
       hudActionState != HUD::EHudActionState::ChooseBuild &&
       hudActionState != HUD::EHudActionState::ChooseTrainUnit) {
      handleHudAction(event, hudActionState, isoMapMousePosition);
    } else {
      // TODO: Change to isometric
      if(type == gengine2d::InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED) {
        if(event.mouseEvent.button == gengine2d::InputEvent::EMouseButton::MB_RIGHT) {
          if(m_unitCommander.isAllSelectedEntitiesUnits()) {
            auto entitySelected = m_unitSelector.selectEntity(isoMapMousePosition);
            if(entitySelected.isValid()) {
              commandUnitsToTarget(entitySelected);
              flag_component_utils::stopTimer(m_positionFlag);
            } else {
              if(!m_unitCommander.isEmptySelection()) {
                commandUnitsToMove(isoMapMousePosition);
                flag_component_utils::showTimedFlag(m_positionFlag,
                                                    gengine2d::vector_utils::convertIsoToCartPoint(isoMapMousePosition));
                spatialpartition_utils::updateSpatialPosition(m_positionFlag,
                                                              base_component_utils::getPosition(m_positionFlag),
                                                              base_component_utils::getSpatialPositionHandle(m_positionFlag),
                                                              m_gameWorld.getEntityManager().getSpatialPartition());
                return;
              }
            }
          } else {
            if(m_unitCommander.numberOfSelectedEntities() == 1 &&
               m_unitCommander.isAllEntitiesSelected([](const anax::Entity& entity) {
                return base_component_utils::isOfType(entity,
                                                      component::BaseEntityType::EEntityType::Building);
               })) {
              auto entitySelected = m_unitSelector.selectEntity(isoMapMousePosition);
              if(entitySelected.isValid()) {
                m_unitCommander.forEachEntitiesSelected([&entitySelected, this]
                (const anax::Entity& entity) {
                  building_component_utils::setTrainTargetEntity(entity, entitySelected);
                  auto& unitTrainFlag = building_component_utils::getUnitTrainFlagEntity(entity);
                  base_component_utils::setAllPosition(unitTrainFlag, gengine2d::Vector2f(-100.f,
                                                                                          -100.f));
                  auto& spatialPartition = m_gameWorld.getEntityManager().getSpatialPartition();
                  spatialpartition_utils::updateSpatialPosition(unitTrainFlag,
                                                                base_component_utils::getPosition(unitTrainFlag),
                                                                base_component_utils::getSpatialPositionHandle(unitTrainFlag),
                                                                spatialPartition);
                });
              } else {
                auto cartMousePosition = gengine2d::vector_utils::convertIsoToCartPoint(isoMapMousePosition);
                m_unitCommander.forEachEntitiesSelected(
                [&cartMousePosition, this](const anax::Entity& entity) {
                  building_component_utils::resetTrainTarget(entity);
                  building_component_utils::setTrainTargetPosition(entity, cartMousePosition);
                  auto& unitTrainFlag = building_component_utils::getUnitTrainFlagEntity(entity);
                  base_component_utils::setAllPosition(unitTrainFlag, cartMousePosition);

                  auto& spatialPartition = m_gameWorld.getEntityManager().getSpatialPartition();
                  spatialpartition_utils::updateSpatialPosition(unitTrainFlag,
                                                                base_component_utils::getPosition(unitTrainFlag),
                                                                base_component_utils::getSpatialPositionHandle(unitTrainFlag),
                                                                spatialPartition);
                });
              }
            }
          }
        }
      }

      if(m_unitSelectionCreator.handleInput(event)) {
        // TODO: Change team
        // TODO: Change to isometric
        auto selection = m_unitSelectionCreator.getSelection();
        auto entitiesSelected = m_unitSelector.selectEntities(Player::EPlayerNumber::PN_PLAYER1,
                                                              selection.first,
                                                              selection.second);

        if(!entitiesSelected.empty()) {
          if(entitiesSelected.size() == 1) {
            // TODO: Change team
            setSelectedEntity(entitiesSelected.front(), Player::EPlayerNumber::PN_PLAYER1);
          } else {
            removeNonUnit(entitiesSelected);
            if(entitiesSelected.size() == 1) {
              setSelectedEntity(entitiesSelected.front(), Player::EPlayerNumber::PN_PLAYER1);
            } else {
              setSelectedEntities(entitiesSelected, Player::EPlayerNumber::PN_PLAYER1);
            }
          }
        } else {
          auto dist = gengine2d::distance(selection.second, selection.first);
          if(dist <= UNIT_SIZE * 4) {
            // TODO: Change to isometric
            auto entitySelected = m_unitSelector.selectEntity(isoMapMousePosition);
            // TODO: Change team
            if(entitySelected.isValid()) {
              const auto& inputState = this->getInputState();
              bool isUnit = base_component_utils::isOfType(entitySelected,
                                                           component::BaseEntityType::EEntityType::Unit);
              bool isBuilding = base_component_utils::isOfType(entitySelected,
                                                               component::BaseEntityType::EEntityType::Building);
              // TODO: Change team
              if((isUnit || isBuilding) &&
                 unit_component_utils::getTeam(entitySelected) == Player::EPlayerNumber::PN_PLAYER1) {
                if(inputState.isKeyboardKeyDown(gengine2d::InputEvent::EKeyboardKey::KK_KEY_LSHIFT) ||
                   inputState.isKeyboardKeyDown(gengine2d::InputEvent::EKeyboardKey::KK_KEY_RSHIFT)) {
                  if(isUnit) {
                    m_unitCommander.addToSelectedEntities(entitySelected);
                  }
                } else {
                  setSelectedEntity(entitySelected,
                                    Player::EPlayerNumber::PN_PLAYER1);
                }
              } else {
                // TODO: Change Team
                m_unitCommander.resetAndClearSelectedEntities();
                m_hud.setSelectedEntity(entitySelected,
                                        Player::EPlayerNumber::PN_PLAYER1,
                                        m_globalResourceManager);
              }
            } else {
              // TODO: Change Team
              m_unitCommander.resetAndClearSelectedEntities();
              m_hud.setSelectedEntity(anax::Entity(),
                                      Player::EPlayerNumber::PN_PLAYER1,
                                      m_globalResourceManager);
            }
          }
        }

        return;
      }
    }
  }

  void InGameState::handleHudAction(const gengine2d::InputEvent& event,
                                    HUD::EHudActionState hudActionState,
                                    const gengine2d::Vector2f& isoMapMousePosition)
  {
    auto type = event.type;

    if(hudActionState == HUD::EHudActionState::Attack) {
      if(type == gengine2d::InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED) {
        auto mouseButtonPressed = event.mouseEvent.button;
        if(mouseButtonPressed == gengine2d::InputEvent::EMouseButton::MB_LEFT) {
          auto entitySelected = m_unitSelector.selectEntity(isoMapMousePosition);
          if(entitySelected.isValid()) {
            m_unitCommander.commandToMoveSelectedEntities(entitySelected);
            m_hud.resetActionState();
          } else {
            m_hud.showMessage("Must select a target");
          }
        } else if(mouseButtonPressed == gengine2d::InputEvent::EMouseButton::MB_RIGHT) {
          m_hud.resetActionState();
        }
      }
    } else if(hudActionState == HUD::EHudActionState::Move) {
      if(type == gengine2d::InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED) {
        auto mouseButtonPressed = event.mouseEvent.button;
        if(mouseButtonPressed == gengine2d::InputEvent::EMouseButton::MB_LEFT) {
          m_unitCommander.commandToMoveSelectedEntities(isoMapMousePosition);
          flag_component_utils::showTimedFlag(m_positionFlag,
                                              gengine2d::vector_utils::convertIsoToCartPoint(isoMapMousePosition));
          m_hud.resetActionState();
        } else if(mouseButtonPressed == gengine2d::InputEvent::EMouseButton::MB_RIGHT) {
          m_hud.resetActionState();
        }
      }
    } else if(hudActionState == HUD::EHudActionState::Build) {
      if(type == gengine2d::InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED) {
        auto mouseButtonPressed = event.mouseEvent.button;
        if(mouseButtonPressed == gengine2d::InputEvent::EMouseButton::MB_LEFT) {
          const BuildingData* buildingData = static_cast<const BuildingData*>(m_hud.getBuildData());
          if(action_acceptor::canBuild(*buildingData,
                                       gengine2d::vector_utils::convertIsoToCartPoint(isoMapMousePosition),
                                       m_gameWorld.getEntityManager())) {
            m_unitCommander.commandToBuildSelectedEntities(*buildingData,
                                                           isoMapMousePosition);
            m_hud.resetActionState();
            // TODO: Change team
            m_hud.setSelectedEntity(m_hud.getSelectedEntity(),
                                    Player::EPlayerNumber::PN_PLAYER1,
                                    m_globalResourceManager);
          } else {
            m_hud.showMessage("Cannot build there");
          }
        } else if(mouseButtonPressed == gengine2d::InputEvent::EMouseButton::MB_RIGHT) {
          m_hud.resetActionState();
          // TODO: Change team
          m_hud.setSelectedEntity(m_hud.getSelectedEntity(),
                                  Player::EPlayerNumber::PN_PLAYER1,
                                  m_globalResourceManager);
        }
      }
    } else if(hudActionState == HUD::EHudActionState::Gather) {
      if(type == gengine2d::InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED) {
        auto mouseButtonPressed = event.mouseEvent.button;
        if(mouseButtonPressed == gengine2d::InputEvent::EMouseButton::MB_LEFT) {
          auto entitySelected = m_unitSelector.selectEntity(isoMapMousePosition);
          if(entitySelected.isValid()) {
            if(base_component_utils::isOfType(entitySelected,
                                              component::BaseEntityType::EEntityType::Resource)) {
              m_unitCommander.commandToMoveSelectedEntities(entitySelected);
            } else {
              m_hud.showMessage("Must select a resource");
            }
            m_hud.resetActionState();
          } else {
            m_hud.showMessage("Must select a resource");
          }
        } else if(mouseButtonPressed == gengine2d::InputEvent::EMouseButton::MB_RIGHT) {
          m_hud.resetActionState();
        }
      }
    } else if(hudActionState == HUD::EHudActionState::ChoosingOneUnit) {
      m_unitCommander.resetAndClearSelectedEntities();
      m_unitCommander.setSelectedEntity(m_hud.getSelectedEntity());
      m_hud.resetActionState();
    } else if(hudActionState == HUD::EHudActionState::ChoosingMultipleUnits) {
      m_unitCommander.resetAndClearSelectedEntities();
      m_unitCommander.setSelectedEntities(m_hud.getSelectedEntities());
      m_hud.resetActionState();
    } else if(hudActionState == HUD::EHudActionState::MinimapMove) {
      auto mousePosition = m_hud.getMinimapMousePosition();
      auto xRatio = MINIMAP_SIZE / m_map.getSize().x;
      auto yRatio = MINIMAP_SIZE / m_map.getSize().y;
      mousePosition.x /= xRatio;
      mousePosition.y /= yRatio;
      m_camera.setCenter(gengine2d::vector_utils::convertCartToIsoPoint(mousePosition * TILE_SIZE));
      m_hud.resetActionState();
    }
  }

  void InGameState::setSelectedEntity(const anax::Entity& entity,
                                      Player::EPlayerNumber team)
  {
    m_hud.setSelectedEntity(entity,
                            Player::EPlayerNumber::PN_PLAYER1,
                            m_globalResourceManager);
    m_unitCommander.setSelectedEntity(entity);
  }

  void InGameState::setSelectedEntities(const gengine2d::vector_t<anax::Entity>& entities,
                                        Player::EPlayerNumber team)
  {
    m_hud.setSelectedEntities(entities,
                              team,
                              m_globalResourceManager);
    m_unitCommander.setSelectedEntities(std::move(entities));
  }

  void InGameState::commandUnitsToMove(const gengine2d::Vector2f& isoMapMousePosition)
  {
    m_unitCommander.commandToMoveSelectedEntities(isoMapMousePosition);
  }

  void InGameState::commandUnitsToTarget(const anax::Entity& target)
  {
    m_unitCommander.commandToMoveSelectedEntities(target);
  }
}
