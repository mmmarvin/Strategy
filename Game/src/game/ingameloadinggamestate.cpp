#include "ingameloadinggamestate.h"

#include "../core/archive_type.h"
#include "../core/configuration.h"
#include "../core/game.h"
#include "../core/rect.h"
#include "../core/utility/general_utility.h"
#include "../core/utility/stream_utility.h"
#include "../core/utility/string_utility.h"
#include "../core/utility/vector_utility.h"

namespace strat
{
namespace
{
  void setRenderCenterX(sf::RectangleShape& rectangle)
  {
    rectangle.setPosition((gengine2d::GameEngine::getInstance().getRenderer().getScreenSize().x / 2.f) -
                          (rectangle.getSize().x / 2.f), rectangle.getPosition().y);
  }

  void setRenderCenterY(sf::RectangleShape& rectangle)
  {
    rectangle.setPosition(rectangle.getPosition().x,
                          (gengine2d::GameEngine::getInstance().getRenderer().getScreenSize().y / 2.f) -
                          (rectangle.getSize().y / 2.f));
  }

  void setRenderPositionX(sf::RectangleShape& rectangle,
                          const gengine2d::string_t& align,
                          int left)
  {
    if(align == "left") {
      rectangle.setPosition(left, rectangle.getPosition().y);
    } else if(align == "right") {
      rectangle.setPosition(gengine2d::GameEngine::getInstance().getRenderer().getScreenSize().x -
                            rectangle.getSize().x -
                            left, rectangle.getPosition().y);
    } else if(align == "center") {
      setRenderCenterX(rectangle);
    } else {
      gengine2d::GameEngine::getInstance().getConsole().printError("Unknown horizontal alignment \"" +
                                                                   align +
                                                                   "\"");
    }
  }

  void setRenderPositionY(sf::RectangleShape& rectangle,
                          const gengine2d::string_t& align,
                          int top)
  {
    if(align == "top") {
      rectangle.setPosition(rectangle.getPosition().x, top);
    } else if(align == "bottom") {
      rectangle.setPosition(rectangle.getPosition().x,
                            gengine2d::GameEngine::getInstance().getRenderer().getScreenSize().y -
                            rectangle.getSize().y -
                            top);
    } else if(align == "center") {
      setRenderCenterY(rectangle);
    } else {
      gengine2d::GameEngine::getInstance().getConsole().printError("Unknown vertical alignment \"" +
                                                                   align +
                                                                   "\"");
    }
  }

  void setRenderPosition(sf::RectangleShape& rectangle,
                         const gengine2d::string_t& align_x,
                         const gengine2d::string_t& align_y,
                         int left,
                         int top)
  {
    setRenderPositionX(rectangle, align_x, left);
    setRenderPositionY(rectangle, align_y, top);
  }

  void loadTexture(sf::RectangleShape& rectangle,
                   sf::Texture& texture,
                   gengine2d::string_t filename,
                   gengine2d::archive_t& archive)
  {
    static const auto archiveFieldLength = std::string_view("archive:").size();

    if(gengine2d::matchBeginning("archive:", filename)) {
      filename = filename.substr(archiveFieldLength,
                                 filename.size() - archiveFieldLength);
      if(archive.fileExists(filename)) {
        auto backgroundTextureData = gengine2d::stream_utils::copyStreamToArray(*archive.getFile(filename)->getInputStream());

        if(texture.loadFromMemory(backgroundTextureData.data(), backgroundTextureData.size())) {
          rectangle.setTexture(&texture);
        } else {
          gengine2d::GameEngine::getInstance().getConsole().printError("Cannot load background texture");
        }
      } else {
        gengine2d::GameEngine::getInstance().getConsole().printError("Cannot load background texture");
      }
    } else {
      if(texture.loadFromFile(filename)) {
        rectangle.setTexture(&texture);
      } else {
        gengine2d::GameEngine::getInstance().getConsole().printError("Cannot load background texture");
      }
    }
  }
}
  InGameLoadingGameState::InGameLoadingGameState(std::unique_ptr<gengine2d::LoadableGameState> gameStateToLoad) :
    gengine2d::LoadingGameState(std::move(gameStateToLoad))
  {
  }

  bool InGameLoadingGameState::init()
  {
    // FIXME: Add Loading State
//    gengine2d::Configuration config;

//    config.addVariable("Background", "src", "", gengine2d::EVariableType::String);
//    config.addVariable("Background", "mode", "stretch", gengine2d::EVariableType::String); // Modes: stretch, center, tile, custom
//    config.addVariable("Background", "align_x", "left", gengine2d::EVariableType::String);
//    config.addVariable("Background", "align_y", "top", gengine2d::EVariableType::String);
//    config.addVariable("Background", "left", "0", gengine2d::EVariableType::Integer);
//    config.addVariable("Background", "top", "0", gengine2d::EVariableType::Integer);
//    config.addVariable("Background", "width", "100", gengine2d::EVariableType::Integer);
//    config.addVariable("Background", "height", "100", gengine2d::EVariableType::Integer);

//    config.addVariable("LoadingBar", "src", "", gengine2d::EVariableType::String);
//    config.addVariable("LoadingBar", "bgsrc", "", gengine2d::EVariableType::String);
//    config.addVariable("LoadingBar", "align_x", "left", gengine2d::EVariableType::String); // Align: left, center, right
//    config.addVariable("LoadingBar", "align_y", "bottom", gengine2d::EVariableType::String); // Align: top, center, bottom
//    config.addVariable("LoadingBar", "left", "0", gengine2d::EVariableType::Integer);
//    config.addVariable("LoadingBar", "top", "70", gengine2d::EVariableType::Integer);
//    config.addVariable("LoadingBar", "width", "0", gengine2d::EVariableType::Integer);
//    config.addVariable("LoadingBar", "height", "50", gengine2d::EVariableType::Integer);
//    config.addVariable("LoadingBar", "bordersize", "0", gengine2d::EVariableType::Integer);
//    config.addVariable("LoadingBar", "bordercolor_r", "0", gengine2d::EVariableType::Integer);
//    config.addVariable("LoadingBar", "bordercolor_g", "0", gengine2d::EVariableType::Integer);
//    config.addVariable("LoadingBar", "bordercolor_b", "0", gengine2d::EVariableType::Integer);
//    config.addVariable("LoadingBar", "bordercolor_a", "0", gengine2d::EVariableType::Integer);

//    gengine2d::archive_t archive;
//    if(archive.open("res/loadingScreen.spak") != gengine2d::archive_t::EOR_SUCCESS) {
//      gengine2d::GameEngine::getInstance().getConsole().printError("Cannot load loading screen");
//      return true;
//    }

//    auto loadingConfig = archive.getFile("loading.conf");
//    if(!loadingConfig) {
//      gengine2d::GameEngine::getInstance().getConsole().printError("Cannot load loading configuration");
//      return true;
//    }

//    if(!config.loadFromStream(*loadingConfig->getInputStream())) {
//      gengine2d::GameEngine::getInstance().getConsole().printError("Cannot parse loading configuration file");
//      return true;
//    }

//    auto backgroundSrc = *config.getString("Background", "src");
//    auto backgroundMode = *config.getString("Background", "mode");
//    auto backgroundAlignX = *config.getString("Background", "align_x");
//    auto backgroundAlignY = *config.getString("Background", "align_y");
//    auto backgroundLeft = *config.getInteger("Background", "left");
//    auto backgroundTop = *config.getInteger("Background", "top");
//    auto backgroundWidth = *config.getInteger("Background", "width");
//    auto backgroundHeight = *config.getInteger("Background", "height");

//    auto loadingBarSrc = *config.getString("LoadingBar", "src");
//    auto loadingBarBgSrc = *config.getString("LoadingBar", "bgsrc");
//    auto loadingBarAlignX = *config.getString("LoadingBar", "align_x");
//    auto loadingBarAlignY = *config.getString("LoadingBar", "align_y");
//    auto loadingBarLeft = *config.getInteger("LoadingBar", "left");
//    auto loadingBarTop = *config.getInteger("LoadingBar", "top");
//    auto loadingBarWidth = *config.getInteger("LoadingBar", "width");
//    auto loadingBarHeight = *config.getInteger("LoadingBar", "height");

//    auto loadingBarBorderSize = *config.getInteger("LoadingBar", "bordersize");
//    auto loadingBarBorderColorR = *config.getInteger("LoadingBar", "bordercolor_r");
//    auto loadingBarBorderColorG = *config.getInteger("LoadingBar", "bordercolor_g");
//    auto loadingBarBorderColorB = *config.getInteger("LoadingBar", "bordercolor_b");
//    auto loadingBarBorderColorA = *config.getInteger("LoadingBar", "bordercolor_a");

//    m_loadingBarWidth = loadingBarWidth;
//    m_loadingBarHeight = loadingBarHeight;

//    auto archiveFieldLength = std::string_view("archive:").size();
//    if(!backgroundSrc.empty()) {
//      loadTexture(m_background, m_backgroundTexture, backgroundSrc, archive);

//      auto renderSize = gengine2d::vector_utils::convertTo<float>(gengine2d::GameEngine::getInstance().getRenderer().getSize());

//      if(backgroundMode == "tile") {
//        m_backgroundTexture.setRepeated(true);
//        m_background.setSize(renderSize);
//        m_background.setPosition(0 ,0);
//        m_background.setTextureRect(gengine2d::Recti(0, 0, renderSize.x, renderSize.y));
//      } else if(backgroundMode == "center") {
//        m_background.setSize(gengine2d::vector_utils::convertTo<float>(m_backgroundTexture.getSize()));
//        setRenderCenterX(m_background);
//        setRenderCenterY(m_background);
//      } else if(backgroundMode == "stretch") {
//        m_background.setSize(renderSize);
//        m_background.setPosition(0 ,0);
//      } else if(backgroundMode == "custom") {
//        m_background.setSize(gengine2d::Vector2f(backgroundWidth, backgroundHeight));
//        setRenderPosition(m_background,
//                          backgroundAlignX,
//                          backgroundAlignY,
//                          backgroundLeft,
//                          backgroundTop);
//      }
//    }

//    if(!loadingBarSrc.empty()) {
//      loadTexture(m_loadingBar, m_loadingBarTexture, loadingBarSrc, archive);
//      m_loadingBar.setSize(gengine2d::Vector2f(loadingBarWidth, loadingBarHeight));
//    }

//    if(!loadingBarBgSrc.empty()) {
//      loadTexture(m_loadingBarBg, m_loadingBarBgTexture, loadingBarBgSrc, archive);
//      m_loadingBarBg.setSize(gengine2d::Vector2f(loadingBarWidth, loadingBarHeight));
//    }

//    setRenderPosition(m_loadingBar,
//                      loadingBarAlignX,
//                      loadingBarAlignY,
//                      loadingBarLeft,
//                      loadingBarTop);

//    setRenderPosition(m_loadingBarBg,
//                      loadingBarAlignX,
//                      loadingBarAlignY,
//                      loadingBarLeft,
//                      loadingBarTop);

//    m_loadingBar.setOutlineThickness(loadingBarBorderSize);
//    m_loadingBar.setOutlineColor(sf::Color(loadingBarBorderColorR,
//                                           loadingBarBorderColorG,
//                                           loadingBarBorderColorB,
//                                           loadingBarBorderColorA));

//    m_loadingBarBg.setOutlineThickness(loadingBarBorderSize);
//    m_loadingBarBg.setOutlineColor(sf::Color(loadingBarBorderColorR,
//                                             loadingBarBorderColorG,
//                                             loadingBarBorderColorB,
//                                             loadingBarBorderColorA));

    return true;
  }

  void InGameLoadingGameState::draw(float)
  {
//    gengine2d::GameEngine::getInstance().getRenderer().draw(m_background);
//    gengine2d::GameEngine::getInstance().getRenderer().draw(m_loadingBarBg);
//    gengine2d::GameEngine::getInstance().getRenderer().draw(m_loadingBar);
  }

  void InGameLoadingGameState::update(float)
  {
    gengine2d::LoadingGameState::update(0.f);

    if(this->isLoading()) {
      float percentage = getGameStateToLoad()->getPercentage();
      percentage /= 100.f;

      m_loadingBar.setSize(gengine2d::Vector2f(percentage * m_loadingBarWidth, m_loadingBarHeight));
    }
  }
}
