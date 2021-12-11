#include "game.h"

#include <cassert>
#include <fstream>
#include <GL/gl.h>
#include <infoware/cpu.hpp>
#include <infoware/version.hpp>
#include <infoware/system.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <misc/cpp/imgui_stdlib.h>
#include "3rdparty/high_resolution_clock.h"
#include "common/variable_common.h"
#include "debug/assert_result.h"
#include "debug/debug_log.h"
#include "resources/default_font.h"
#include "utility/converter_utility.h"
#include "utility/string_utility.h"
#include "utility/stl_utility.h"
#include "config.h"
#include "console_err_tags.h"
#include "define.h"

namespace gengine2d
{
  void Game::FontDeleter::operator()(game_internal::Renderer::Font& font)
  {
    GameEngine::getInstance().getRenderer().removeFont(font);
  }

  Game::ResourceManager::ResourceManager() :
    m_fontLookup("FONT"),
    m_textureLookup("TEXTURE")//,
//    m_soundLookup("SOUND"),
//    m_musicLookup("MUSIC")
  {
  }

//  Game::PackLoaders::PackLoaders() :
//    m_texturePackLoader(nullptr),
//    m_soundPackLoader(nullptr)
//  {
//  }

//  TexturePackLoader& Game::PackLoaders::getTexturePackLoader()
//  {
//    auto* currentState = GameEngine::getInstance().getCurrentState();
//    m_texturePackLoader.setState(currentState);

//    return m_texturePackLoader;
//  }

//  SoundPackLoader& Game::PackLoaders::getSoundPackLoader()
//  {
//    auto* currentState = GameEngine::getInstance().getCurrentState();
//    m_soundPackLoader.setState(currentState);

//    return m_soundPackLoader;
//  }

  Game::Game() :
//    m_testMusic(nullptr),
    m_statesLocked(false),
    m_consoleMode(false),
    m_canGoConsole(false),
    m_initialized(false)
  {
    m_debugTimes.emplace_back(0);
    m_debugTimes.emplace_back(0);
    m_debugTimes.emplace_back(0);
  }

  Game::~Game()
  {
    if(m_initialized) {
      ImGui::SFML::Shutdown();
    }
  }

  bool Game::init(string_t title,
                  bool canGoConsole,
                  std::initializer_list<string_t> convertToReadOnly,
                  std::initializer_list<std::pair<string_t, string_t>> defaultValues)
  {
    m_canGoConsole = canGoConsole;

    assert(!m_initialized);
    auto& console = getConsole();

    console.printLine(gengine2d_TITLE + " v." + config_utils::versionToString(gengine2d_VERSION_MAJOR,
                                                                              gengine2d_VERSION_MINOR,
                                                                              gengine2d_VERSION_PATCH));
    console.printLine("");
    console.printLine("Setting up game engine...");
    addDefaultGameVariables();
    changeDefaultGameVariableValues(defaultValues);
    changeDefaultGameVariableReadOnly(convertToReadOnly);
    loadVariables();
    console.print("Done.");

    console.printLine("Creating game screen...");
    auto windowWidth = *m_variableManager.getInteger("client.window.width");
    auto windowHeight = *m_variableManager.getInteger("client.window.height");
    auto windowFullscreen = *m_variableManager.getBool("client.window.fullscreen");
    auto windowVsync = *m_variableManager.getBool("client.window.vsync");
    auto colorDepth = *m_variableManager.getInteger("client.window.color_depth");

    // TODO: Add stencil buffer, and input handler
    m_renderer.init(console,
                    title,
                    windowWidth,
                    windowHeight,
                    colorDepth,
                    windowFullscreen,
                    windowVsync,
                    false,
                    static_cast<void*>(&m_inputHandler));

    console.printLine("Loading default resources...");
    loadDefaultResources();
    console.print("Done");

    m_debugTextStyle.font = &getResourceManager().getFontLookup().getResource("FONT_DEFAULT");
    m_debugTextStyle.fillColor = Color::White;
    m_debugTextStyle.outlineColor = Color::Black;
    m_debugTextStyle.outlineThickness = 1;

    m_renderer.ImGuiHandleInit();
    ImGui::GetIO().IniFilename = "";

    m_initialized = true;
    return true;
  }

  void Game::addStateInstance(std::unique_ptr<GameState> state)
  {
    if(!m_statesLocked) {
      m_statesCache.emplace_back(std::make_pair(EStateAction::Push,
                                                std::move(state)));
      return;
    }

    DEBUG_DEV_WARNING_LOG("Cannot add a new state, because states is locked");
  }

  void Game::addStateInstanceInitialzied(std::unique_ptr<GameState> state)
  {
    if(!m_statesLocked) {
      m_statesCache.emplace_back(std::make_pair(EStateAction::PushInitialized,
                                                std::move(state)));
      return;
    }

    DEBUG_DEV_WARNING_LOG("Cannot add a new state, because states is locked");
  }

  void Game::popState()
  {
    if(!m_statesLocked) {
      m_statesCache.emplace_back(std::make_pair(EStateAction::Pop,
                                                std::unique_ptr<GameState>(nullptr)));
      return;
    }

    DEBUG_DEV_WARNING_LOG("Cannot pop state, because states is locked");
  }

  void Game::apply(string_t cmd)
  {
    auto& console = getConsole();
    auto runStateApply = [&console](GameState* currentState,
                                    const vector_t<string_t>& tokens,
                                    const string_t& name)
    {
      if(!currentState->apply(tokens)) {
        console.printError(CETAG_CONSOLE + "Unknown command \"" + name + "\"");
      }
    };

    auto tokens = tokenize(cmd, { '\"' });
    auto numberOfTokens = tokens.size();

    if(numberOfTokens > 0) {
      auto key = toLower(tokens[0]);
      const auto& commandList = command_utils::getCommandList();
      if(auto findIt = commandList.find(key); findIt != commandList.end()) {
        findIt->second(key, tokens);
      } else {
      /*else if(name == "play_music") {
        if(hasRequiredNumberOfTokens(numberOfTokens, 2, tokens[0])) {
          m_testMusic.stop();

          auto loadRes = getAudioManager().loadMusic(tokens[1], tokens[1]);
          if(loadRes.first) {
            m_testMusic = std::move(loadRes.second);
            m_testMusic.play();
            GameEngine::getInstance().getConsole().printLine(CETAG_CONSOLE +
                                                             "Playing music \"" +
                                                             tokens[1] +
                                                             "\"...");
          }
        }
      } else if(name == "stop_music") {
        if(hasRequiredNumberOfTokens(numberOfTokens, 1, tokens[0])) {
          if(m_testMusic.getStatus() == game_internal_type::AudioManager::Music::EStatus::Playing) {
            m_testMusic.stop();
            GameEngine::getInstance().getConsole().printLine(CETAG_CONSOLE +
                                                             "Stopped music.");
          } else {
            GameEngine::getInstance().getConsole().printLine(CETAG_CONSOLE +
                                                             "Music not playing.");
          }
        }
      } else if(name == "client.music.volume") {
        if(hasRequiredNumberOfTokens(numberOfTokens, 2, tokens[0])) {
          auto& musicLookup = getResourceManager().getMusicLookup();
          auto volumeValue = stringTo<float>(tokens[1]);
          volumeValue = std::clamp(volumeValue, 0.f, 100.f);

          // TODO: Add client.music.volume to variable manager
          // so future added music's volume will be set to this
          for(std::size_t i = 0, isize = musicLookup.size(); i < isize; ++i) {
            auto& music = musicLookup.m_resourceLookup.getResourceByIndex(i);
            music.setVolume(volumeValue);
          }
        }
      } */
        if(m_variableManager.exists(key)) {
          if(numberOfTokens == 1) {
            console.printLine("\"" +
                              tokens[0] +
                              "\" = \"" +
                              m_variableManager.getValueString(key) +
                              "\"");
          } else if(numberOfTokens == 2) {
            auto setResult = variable_common::setVariable(m_variableManager, key, tokens[1]);
            if(setResult != VariableManager::ESetReturn::Success &&
               setResult != VariableManager::ESetReturn::InvalidType &&
               setResult != VariableManager::ESetReturn::IsReadOnly) {
              runStateApply(m_currentState, tokens, tokens[0]);
            }
          } else {
            console.printError("Invalid number of arguments for variable command");
          }
        } else {
          runStateApply(m_currentState, tokens, tokens[0]);
        }
      }
    }
  }

  void Game::run()
  {
    if(!m_initialized) {
      GameLogger::getInstance().writeError("[CRITICAL] Cannot run game, because there was an error initializing the game engine");
      return;
    }

    float accumulator = 0.f, timeStep = 1.f / 64.f;
    mar::high_resolution_clock gameClock, fpsClock, debugClock;
    unsigned int fps = 0, numberOfFrames = 0;

    while(m_renderer.run()) {
      float elapsed = gameClock.restart().as_seconds();
      if(elapsed >= 0.25f) {
        elapsed = 0.25f;
      }

      updateStates();
      if(m_states.empty()) {
        break;
      }

      m_renderer.ImGuiHandleUpdate(timeStep);
      accumulator += elapsed;
      while(accumulator >= timeStep) {
        debugClock.restart();
        update(timeStep);
        m_debugTimes[0] = debugClock.get_elapsed().as_microseconds();

        debugClock.restart();
        handleInput();
        m_debugTimes[1] = debugClock.get_elapsed().as_microseconds();

        accumulator -= timeStep;
      }
      interpolate(timeStep, accumulator);

      ++numberOfFrames;
      if(auto fpsElapsed = fpsClock.get_elapsed().as_seconds(); fpsElapsed >= 1.f) {
        fps = numberOfFrames / fpsElapsed;
        numberOfFrames = 0;
        fpsClock.restart();
      }

      debugClock.restart();
      // TODO: Change to black
      m_renderer.clear();
      draw(accumulator / timeStep, fps);
      m_renderer.display();
      m_debugTimes[2] = debugClock.get_elapsed().as_microseconds();
    }
    DEBUG_LOG("gengine2d has reached the end.");
  }

  void Game::quit()
  {
    m_statesCache.emplace_back(EStateAction::Quit, std::unique_ptr<GameState>(nullptr));
  }

  void Game::setPrevStateMessage(string_t msg)
  {
    if(m_states.size() > 2) {
      stl_utils::next(m_states.rbegin(), 1)->get()->setMessage(std::move(msg));
    }
  }

  vector_t<string_t> Game::getCommandList() const
  {
    auto commandList = m_variableManager.getVariableList();
    const auto& engineCommandList = command_utils::getCommandList();
    commandList.reserve(commandList.size() + engineCommandList.size());
    for(const auto& [key, callBackFunction] : engineCommandList) {
      commandList.push_back(key);
    }

    auto stateCommandList = getCurrentState()->getCommandList();
    commandList.insert(commandList.end(), stateCommandList.begin(), stateCommandList.end());

    return commandList;
  }

  void Game::printPCInfo()
  {
    auto& console = getConsole();
    auto lines = tokenize2(getPCInfo(), '\n');
    for(const auto& line : lines) {
      console.printLine(line);
    }
  }

  string_t Game::getPCInfo()
  {
    static constexpr auto cpu_architecture_name = [](iware::cpu::architecture_t architecture) -> string_t {
      switch(architecture) {
        case iware::cpu::architecture_t::x64:
          return "x64";
        case iware::cpu::architecture_t::arm:
          return "ARM";
        case iware::cpu::architecture_t::itanium:
          return "Itanium";
        case iware::cpu::architecture_t::x86:
          return "x86";
        default:
          break;
      }

      return "Unknown";
    };

    const auto cpu_quantities = iware::cpu::quantities();
    string_t cpu_info = string_t("===================\n") +
                        string_t("CPU\n") +
                        string_t("===================\n") +
                        string_t("\tNumber of logical CPUs: ") + stl_utils::to_string(cpu_quantities.logical) + "\n" +
                        string_t("\tNumber of physics CPUs: ") + stl_utils::to_string(cpu_quantities.physical) + "\n" +
                        string_t("\tArchitecture: ") + cpu_architecture_name(iware::cpu::architecture()) + "\n" +
                        string_t("\tFrequency: ") + stl_utils::to_string(iware::cpu::frequency()) + " Hz\n" +
                        string_t("\tModel: ") + iware::cpu::model_name() + "\n" +
                        string_t("\tVendor: ") + iware::cpu::vendor() + "\n" +
                        string_t("\tVendor ID: ") + iware::cpu::vendor_id() + "\n";

    string_t gpu_info = string_t("===================\n") +
                        string_t("GPU\n") +
                        string_t("===================\n") +
                        string_t("\tVendor: ") + string_t(reinterpret_cast<const char*>(glGetString(GL_VENDOR))) + "\n" +
                        string_t("\tDriver: ") + string_t(reinterpret_cast<const char*>(glGetString(GL_RENDERER))) + "\n" +
                        string_t("\tVersion: ") + string_t(reinterpret_cast<const char*>(glGetString(GL_VERSION))) + "\n" +
                        string_t("\tShader Version: " + string_t(reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION)))) + "\n" +
                        string_t("\tGame Resolution: ") + stl_utils::to_string(m_renderer.getScreenSize().x)  + "x" + stl_utils::to_string(m_renderer.getScreenSize().y) + "\n";

    const auto mem = iware::system::memory();
    const auto osInfo = iware::system::OS_info();
    string_t system_info = string_t("===================\n") +
                           string_t("System\n") +
                           string_t("===================\n") +
                           string_t("* RAM:\n") +
                           string_t("\tPhysical:\n") +
                           string_t("\t\tAvailable: ") + stl_utils::to_string(mem.physical_available) + " B\n" +
                           string_t("\t\tTotal: ") + stl_utils::to_string(mem.physical_total) + " B\n" +
                           string_t("\tVirtual:\n") +
                           string_t("\t\tAvailable: ") + stl_utils::to_string(mem.virtual_available) + " B\n" +
                           string_t("\t\tTotal: ") + stl_utils::to_string(mem.virtual_total) + " B\n" +
                           string_t("* OS:\n") +
                           string_t("\tName: ") + osInfo.name + "\n" +
                           string_t("\tFull Name: ") + osInfo.full_name + "\n" +
                           string_t("\tVersion: ") + stl_utils::to_string(osInfo.major) + "." + stl_utils::to_string(osInfo.minor) + stl_utils::to_string(osInfo.patch) + " build " + stl_utils::to_string(osInfo.build_number) + "\n";

    return cpu_info + " \n" + gpu_info + " \n" + system_info;
  }

  void Game::draw(float alpha, unsigned int fps)
  {
    m_currentState->draw(alpha);
    if(m_currentState->hasMessage()) {
      drawMessages();
    }
    if(m_consoleMode) {
      getConsole().draw();
    }
    m_renderer.ImGuiHandleRender();

    m_renderer.reset2DCamera();
    if(*m_varFlagDebugShowFPS) {
      m_debugTextStyle.characterSize = 24;
      m_debugTextStyle.outlineThickness = 2.f;
      m_debugTextStyle.outlineColor = Color::Black;
      m_debugTextStyle.style = Text2DStyle::TS_BOLD;
      m_renderer.draw2DText(gengine2d::Vector2f(5, 5),
                            "FPS: " + stl_utils::to_string(fps),
                            m_debugTextStyle);
    }
    if(*m_varFlagDebugShowBasicProf) {
      auto profileStr = "FPS : " + stl_utils::to_string(fps) + "\n" +
                        "Draw time: " + stl_utils::to_string(m_debugTimes[2]) +
                        "ms \nUpdate time: " + stl_utils::to_string(m_debugTimes[0]) +
                        "ms \nInput time: " + stl_utils::to_string(m_debugTimes[1]) + " ms";
      auto textBounds = m_renderer.calculate2DTextBounds(profileStr,
                                                         m_debugTextStyle);

      m_debugTextStyle.characterSize = 12;
      m_debugTextStyle.outlineThickness = 1.f;
      m_debugTextStyle.outlineColor = Color::Black;
//      m_debugTextStyle.style = Text2DStyle::TS_BOLD;
      m_renderer.draw2DText(Vector2<render::position_point_2d_t>(5,
                                                                 m_renderer.getScreenSize().y - textBounds.y - 5),
                            profileStr,
                            m_debugTextStyle);
    }
  }

  void Game::update(float dt)
  {
    m_currentState->update(dt);
  }

  void Game::interpolate(float dt, float accumulator)
  {
    m_currentState->interpolate(dt, accumulator);
  }

  void Game::handleInput()
  {
    InputEvent event;
    while(m_inputHandler.handleInput(event)) {
      m_inputHandler.ImGuiHandleInput();

      if(event.type == InputEvent::EEventType::ET_KEYBOARD_KEY_PRESSED &&
         event.keyboardEvent.key == InputEvent::EKeyboardKey::KK_KEY_TILDA &&
         m_canGoConsole) {
        m_consoleMode = !m_consoleMode;
      }

      if(m_consoleMode) {
        m_console.handleInput(event);
      } else {
        m_currentState->handleInput(event);
      }
    }
  }

  void Game::drawMessages()
  {
    auto msg = m_currentState->getMessage();
    if(ImGui::BeginPopupModal("Message", 0, ImGuiWindowFlags_NoCollapse |
                                            ImGuiWindowFlags_NoResize |
                                            ImGuiWindowFlags_NoMove)) {
      ImGui::Text(msg.c_str());
      ImGui::Button("Ok");
      if(ImGui::IsItemClicked()) {
        m_currentState->popMessage();
        ImGui::CloseCurrentPopup();
      }
      ImGui::EndPopup();
    }

    if(!ImGui::IsPopupOpen("Message") && !ImGui::IsPopupOpen("Console")) {
      ImGui::OpenPopup("Message");
    }
  }

  void Game::lockStates()
  {
    m_statesLocked = true;
  }

  void Game::unlockStates()
  {
    m_statesLocked = false;
  }

  void Game::updateStates()
  {
    auto& console = getConsole();

    if(!m_statesCache.empty()) {
      console.printLine("Updating states... " + stl_utils::to_string(m_statesCache.size()) + " new states to read...");
      for(auto&& [action, state] : m_statesCache) {
        if(action == EStateAction::Push) {
          console.printLine("Pushing state \"" + state->getDebugName() + "\"");
          m_states.push_back(std::move(state));

          // lock the states, so that no states can be added during initialization of the current state
          lockStates();
          if(!m_states.back()->init()) {
            m_states.pop_back();
          }
          unlockStates();
        } else if(action == EStateAction::PushInitialized) {
          console.printLine("Pushing state \"" + state->getDebugName() + "\"");
          m_states.push_back(std::move(state));
        } else if(action == EStateAction::Pop) {
          if(!m_states.empty()) {
            console.printLine("Popping state... States left: " + stl_utils::to_string(m_states.size() - 1));
            m_states.pop_back();
          }
        } else if(action == EStateAction::Quit) {
          console.printLine("Cleaning states...");
          while(!m_states.empty()) {
            m_states.pop_back();
          }
        }
      }

      m_statesCache.clear();
      if(!m_states.empty()) {
        m_currentState = m_states.back().get();
        console.printLine("Changing state to \"" +
                          m_currentState->getDebugName() +
                          "\"...");
      } else {
        m_currentState = nullptr;
      }
    }
  }

  void Game::addDefaultGameVariables()
  {
    assert_result(m_variableManager.addVariable("client.window.width", "800", EVariableType::Integer));
    assert_result(m_variableManager.addVariable("client.window.height", "600", EVariableType::Integer));
    assert_result(m_variableManager.addVariable("client.window.fullscreen",
                                                "false",
                                                EVariableType::Boolean));
    assert_result(m_variableManager.addVariable("client.window.vsync", "false", EVariableType::Boolean));
    assert_result(m_variableManager.addVariable("client.window.color_depth",
                                                "32",
                                                EVariableType::Integer));

    assert_result(m_variableManager.addVariable("client.debug.show_fps",
                                                "false",
                                                EVariableType::Boolean));
    assert_result(m_variableManager.addVariable("client.debug.show_basic_prof",
                                                "true",
                                                EVariableType::Boolean));

    m_varFlagDebugShowFPS = m_variableManager.getBool("client.debug.show_fps");
    m_varFlagDebugShowBasicProf = m_variableManager.getBool("client.debug.show_basic_prof");
  }

  void Game::changeDefaultGameVariableReadOnly(std::initializer_list<string_t> convertToReadOnly)
  {
    for(auto name : convertToReadOnly) {
      assert_result(m_variableManager.setReadOnly(name, true));
    }
  }

  void Game::changeDefaultGameVariableValues(std::initializer_list<std::pair<string_t, string_t>> defaultValues)
  {
    for(auto [name, value] : defaultValues) {
      assert_result(m_variableManager.setVariable(name, value));
    }
  }

  void Game::loadVariables()
  {
    game_utils::loadVariables(CUSTOM_CONFIG_FILENAME, m_variableManager);
  }

  void Game::loadDefaultResources()
  {
    // TODO: Make font load from memory
    auto newFont = m_renderer.loadFont("FONT_DEFAULT", "fonts/DejaVuSans.ttf");
    if(newFont.first) {
      m_resourceManager.getFontLookup().m_resourceLookup.addResource("FONT_DEFAULT",
                                                                     std::move(newFont.second));
    } else {
      std::abort();
    }
  }
namespace game_utils
{
  void loadVariables(const string_t& filename, VariableManager& variableManager)
  {
    auto& console = GameEngine::getInstance().getConsole();

    std::ifstream in(filename);
    if(in.is_open()) {
      console.printLine("Loading autorun.cfg...");

      string_t line;
      while(std::getline(in, line)) {
        auto tokens = tokenize(std::move(line), { '\"' });
        if(tokens.size() != 2) {
          if(tokens.size() >= 1) {
            console.printError(CETAG_CONSOLE + "Unknown variable \"" + tokens[0] + "\"");
          }
          continue;
        }

        auto name = tokens[0];
        auto value = tokens[1];
        auto setResult = variableManager.setVariable(name, value);
        switch(setResult) {
        case VariableManager::ESetReturn::InvalidName:
          console.printError(CETAG_CONSOLE + "Unknown variable \"" + name + "\"");
          break;
        case VariableManager::ESetReturn::InvalidType:
          console.printError(CETAG_CONSOLE + "Cannot set \"" + name + "\" to \"" + value + "\": Types don't match");
          break;
        case VariableManager::ESetReturn::IsReadOnly:
          console.printLine("Cannot set \"" + name + "\" to \"" + value + "\": Variable is read-only");
          break;
        default:
          break;
        }
      }
    }
  }
}
}
