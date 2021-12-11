#include "gamestate.h"

#include <filesystem>
#include <fstream>
////#include "../loader/texturepackloader.h"
#include "common/console_message_common.h"
#include "debug/debug_log.h"
#include "utility/stream_utility.h"
#include "utility/pak_utility.h"
#include "archive_type.h"
#include "game.h"

namespace gengine2d
{
namespace
{
  void addResource(fast_unordered_set_t<string_t>& resourceHolder,
                   const string_t& resourceName)
  {
    resourceHolder.emplace(resourceName);
  }

  bool resourceExists(const fast_unordered_set_t<string_t>& resourceHolder,
                      const string_t& resourceName)
  {
    return resourceHolder.find(resourceName) != resourceHolder.end();
  }
}
  GameState::GameState(std::initializer_list<game_internal::Renderer::Camera2D*> cameras,
                       std::initializer_list<std::tuple<string_t, bool, unsigned int, std::function<void(GameState*)>>> initialKeyMapped) :
    m_controlMap(initialKeyMapped),
    m_inputState(cameras.size()),
    m_mousePositionOnviews(cameras.begin(), cameras.end())
  {
  }

  GameState::~GameState()
  {
    auto& fontLookup = GameEngine::getInstance().getResourceManager().getFontLookup();
    for(const auto& fontName : m_addedFonts) {
      fontLookup.m_resourceLookup.removeResource(fontName);
    }

    auto& textureLookup = GameEngine::getInstance().getResourceManager().getTextureLookup();
    for(const auto& textureName : m_addedTextures) {
      textureLookup.m_resourceLookup.removeResource(textureName);
    }
  }

  void GameState::handleInput(const InputEvent& event)
  {
    switch(event.type) {
    case InputEvent::EEventType::ET_MOUSE_MOVED: {
      auto& renderer = GameEngine::getInstance().getRenderer();
      auto mousePosition = GameEngine::getInstance().getInputHandler().getMousePosition();

      assert(m_mousePositionOnviews.size() + 1 == m_inputState.m_mousePositions.size());

      m_inputState.m_mousePositions[0] = renderer.get2DGUICamera().getPositionInTermsOf(mousePosition);
      for(std::size_t i = 0, isize = m_mousePositionOnviews.size(); i < isize; ++i) {
        m_inputState.m_mousePositions[i + 1] = m_mousePositionOnviews[i]->getPositionInTermsOf(mousePosition);
      }
      break;
    }
    case InputEvent::EEventType::ET_KEYBOARD_KEY_PRESSED:
      m_inputState.m_keyboardKeyState[event.keyboardEvent.key] = true;
      break;
    case InputEvent::EEventType::ET_KEYBOARD_KEY_RELEASED:
      m_inputState.m_keyboardKeyState[event.keyboardEvent.key] = false;
      break;
    case InputEvent::EEventType::ET_MOUSE_BUTTON_PRESSED:
      m_inputState.m_mouseButtonState[event.mouseEvent.button] = true;
      break;
    case InputEvent::EEventType::ET_MOUSE_BUTTON_RELEASED:
      m_inputState.m_mouseButtonState[event.mouseEvent.button] = false;
      break;
    default:
      break;
    }
  }

  void GameState::setMessage(string_t msg)
  {
    m_messages.push_back(std::move(msg));
  }

  bool GameState::hasMessage() const
  {
    return !m_messages.empty();
  }

  void GameState::popMessage()
  {
    m_messages.pop_back();
  }

  const string_t& GameState::getMessage() const
  {
    return m_messages.back();
  }

  bool GameState::addFont(const string_t& name, const string_t& filename)
  {
    std::ifstream in(filename, std::ios::binary);
    if(in.is_open()) {
      return addFont(name, in);
    }

    return false;
  }

  bool GameState::addFont(const string_t& name, std::istream& stream)
  {
    if(!resourceExists(m_addedFonts, name)) {
      auto& fontLookup = GameEngine::getInstance().getResourceManager().getFontLookup();
      auto newFont = GameEngine::getInstance().getRenderer().loadFont(name, stream);
      if(newFont.first &&
         fontLookup.m_resourceLookup.addResource(name, std::move(newFont.second))) {
        addResource(m_addedFonts, name);
        return true;
      }
    }

    return false;
  }

  bool GameState::addTexture(const string_t& name, const string_t& filename)
  {
    std::ifstream in(filename, std::ios::binary);
    if(in.is_open()) {
      return addTexture(name, in);
    }

    return false;
  }

  bool GameState::addTexture(const string_t& name, std::istream& stream)
  {
    if(!resourceExists(m_addedTextures, name)) {
      auto& textureLookup = GameEngine::getInstance().getResourceManager().getTextureLookup();

      auto newTexture = GameEngine::getInstance().getRenderer().loadTexture(name, stream);
      if(newTexture.first &&
         textureLookup.m_resourceLookup.addResource(name, std::move(newTexture.second))) {
        addResource(m_addedTextures, name);
        return true;
      }
    }

    return false;
  }

  bool GameState::addTextureFromPack(const string_t& textureFilename,
                                     const string_t& texturePackFilename)
  {
    archive_t packArchive;
    if(packArchive.open(texturePackFilename) == archive_t::EOR_SUCCESS &&
       packArchive.fileExists(textureFilename)) {
      return addTexture(pak_utils::getTextureName(textureFilename, texturePackFilename),
                        *packArchive.getFile(textureFilename)->getInputStream());
    }

    GameEngine::getInstance().getConsole().printWarning(console_message_common::generateMessage("PakLoader",
                                                                                                "Cannot load \"" + textureFilename + "\" from \"" + texturePackFilename + "\""));
    return false;
  }

  bool GameState::addTexturesToLookup(const string_t& filename)
  {
    return m_textureLookup.loadFromFile(filename);
  }

  bool GameState::addTexturesToLookup(const string_t& filename, std::istream& stream)
  {
    return m_textureLookup.loadFromStream(filename, stream);
  }

  bool GameState::addTextureFromPackToLookup(const string_t& textureFilename,
                                             const string_t& texturePackFilename)
  {
    return m_textureLookup.loadIndividual(textureFilename, texturePackFilename);
  }

//  bool State::addSound(const string_t& name, const string_t& filename)
//  {
//    std::ifstream in(filename, std::ios::binary);
//    if(in.is_open()) {
//      return addSound(name, in);
//    }

//    return false;
//  }

//  bool State::addSound(const string_t& name, std::istream& stream)
//  {
//    if(!resourceExists(m_addedSounds, name)) {
//      auto& audioManager = GameEngine::getInstance().getAudioManager();
//      auto& soundLookup = GameEngine::getInstance().getResourceManager().getSoundLookup();

//      auto newSound = audioManager.loadSound(name, stream);
//      if(newSound.first &&
//         soundLookup.m_resourceLookup.addResource(name, std::move(newSound.second))) {
//        addResource(m_addedSounds, name);
//        return true;
//      }
//    }

//    return false;
//  }

//  bool State::loadSoundFromPack(const string_t& soundName, const string_t& soundPackFilename)
//  {
//    auto& audioLookup = GameEngine::getInstance().getResourceManager().getSoundLookup();
//    if(audioLookup.exists(soundName)) {
//      GameEngine::getInstance().getConsole().printWarning(CETAG_RES_LOADER +
//                                                          "Sound \"" +
//                                                          soundName +
//                                                          "\" already loaded. Skipping...");

//      return false;
//    }

//    return GameEngine::getInstance().getPackLoaders().getSoundPackLoader().loadFromFile(soundPackFilename,
//                                                                                        soundName);
//  }

//  bool State::addMusic(const string_t& name, const string_t& filename)
//  {
//    std::ifstream in(filename, std::ios::binary);
//    if(in.is_open()) {
//      return addMusic(name, in);
//    }

//    return false;
//  }

//  bool State::addMusic(const string_t& name, std::istream& stream)
//  {
//    if(!resourceExists(m_addedMusics, name)) {
//      auto& audioManager = GameEngine::getInstance().getAudioManager();
//      auto& musicLookup = GameEngine::getInstance().getResourceManager().getMusicLookup();

//      auto newMusic = audioManager.loadMusic(name, stream);
//      if(newMusic.first &&
//         musicLookup.m_resourceLookup.addResource(name, std::move(newMusic.second))) {
//        addResource(m_addedMusics, name);
//        return true;
//      }
//    }

//    return false;
//  }

//  bool State::loadMusicFromPack(const string_t& musicName, const string_t& musicPackFilename)
//  {
//    assert(false && "This function does not work");
//    return false;
//  }
}
