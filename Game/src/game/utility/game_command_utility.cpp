#include "game_command_utility.h"

#include "../../core/common/variable_common.h"
#include "../../core/utility/converter_utility.h"
#include "../../core/utility/stl_utility.h"
#include "../../core/utility/string_utility.h"
#include "../ingamestate.h"

namespace strat
{
namespace command_utils
{
  const gengine2d::fast_unordered_map_t<gengine2d::string_t, std::function<bool(const gengine2d::string_t&, const gengine2d::vector_t<gengine2d::string_t>&, InGameState&)>>& getCommandList()
  {
    static constexpr auto spawnCommand = [](const gengine2d::string_t& token,
                                            const gengine2d::vector_t<gengine2d::string_t>& tokens,
                                            InGameState& ingameState) {
      if(gengine2d::variable_common::hasRequiredNumberOfTokens(tokens.size(), 2, token)) {
        auto value = tokens[1];
        if(ingameState.m_unitDataLookup.exists(value)) {
          const auto& unitData = ingameState.m_unitDataLookup.getResource(value);
          const auto& mousePosition = ingameState.getInputState().getMousePosition(1);

          // TODO: Fix this so that there is a castle?
          auto spawnPosition = gengine2d::vector_utils::convertIsoToCartPoint(mousePosition);
          ingameState.m_gameWorld.getEntityManager().createUnit(unitData,
                                                                Player::EPlayerNumber::PN_PLAYER1,
                                                                spawnPosition,
                                                                anax::Entity());
          gengine2d::GameEngine::getInstance().getConsole().printLine("Spawned " +
                                                                      unitData.name +
                                                                      " at [" +
                                                                      gengine2d::stl_utils::to_string(spawnPosition.x) +
                                                                      ", " +
                                                                      gengine2d::stl_utils::to_string(spawnPosition.y) +
                                                                      "]");
        }
      }

      return true;
    };

    static constexpr auto resourceCommand = [](const gengine2d::string_t& token,
                                               const gengine2d::vector_t<gengine2d::string_t>& tokens,
                                               InGameState& ingameState,
                                               GlobalResource::EGlobalResouceType resourceType) {
      if(gengine2d::variable_common::hasRequiredNumberOfTokens(tokens.size(), 3, token)) {
        auto value = tokens[1];
        auto team = gengine2d::toLower(tokens[2]);

        auto intValue = gengine2d::converter_utils::stringTo<int>(value);
        if(intValue > 0) {
          if(team.size() == 7 && gengine2d::partialMatch("player", team)) {
            auto teamNumber = gengine2d::converter_utils::stringTo<int>(gengine2d::string_t(1, team.back()));
            if(teamNumber > 0) {
              ingameState.m_globalResourceManager.getGlobalResource().addResource(static_cast<Player::EPlayerNumber>(teamNumber - 1),
                                                                                  resourceType,
                                                                                  1000);
              return true;
            } else {
              gengine2d::GameEngine::getInstance().getConsole().printError("Invalid team number " + team);
            }
          } else {
            gengine2d::GameEngine::getInstance().getConsole().printError("Invalid team " + team);
          }
        } else {
          gengine2d::GameEngine::getInstance().getConsole().printError("Invalid value " + value);
        }
      }

      return false;
    };

    static constexpr auto giveResourceCommand = [](const gengine2d::string_t& token,
                                                   const gengine2d::vector_t<gengine2d::string_t>& tokens,
                                                   InGameState& ingameState) {
      if(gengine2d::variable_common::hasRequiredNumberOfTokens(tokens.size(), 4, token)) {
        auto resource = gengine2d::toLower(tokens[1]);
        auto newTokens = gengine2d::vector_t<gengine2d::string_t>{ "", tokens[2], tokens.back() };
        if(resource == "food") {
          resourceCommand(token, newTokens, ingameState, GlobalResource::GR_RES_FOOD);
        } else if(resource == "gold") {
          resourceCommand(token, newTokens, ingameState, GlobalResource::GR_RES_GOLD);
        } else if(resource == "stone") {
          resourceCommand(token, newTokens, ingameState, GlobalResource::GR_RES_STONE);
        } else if(resource == "wood") {
          resourceCommand(token, newTokens, ingameState, GlobalResource::GR_RES_WOOD);
        }
      }

      return true;
    };

    static constexpr auto giveAllResourceCommand = [](const gengine2d::string_t& token,
                                                      const gengine2d::vector_t<gengine2d::string_t>& tokens,
                                                      InGameState& ingameState) {
      if(gengine2d::variable_common::hasRequiredNumberOfTokens(tokens.size(), 2, token)) {
        auto newTokens = gengine2d::vector_t<gengine2d::string_t>{ "", "9999", tokens.back() };
        if(resourceCommand(token, newTokens, ingameState, GlobalResource::GR_RES_FOOD) &&
           resourceCommand(token, newTokens, ingameState, GlobalResource::GR_RES_GOLD) &&
           resourceCommand(token, newTokens, ingameState, GlobalResource::GR_RES_STONE) &&
           resourceCommand(token, newTokens, ingameState, GlobalResource::GR_RES_WOOD)) {
          // Stupid logic here was purposely done
          return true;
        }
      }

      return true;
    };

    static const gengine2d::fast_unordered_map_t<gengine2d::string_t, std::function<bool(const gengine2d::string_t&, const gengine2d::vector_t<gengine2d::string_t>&, InGameState&)>> COMMAND_LIST =
    {
      { "spawn", spawnCommand },
      { "give", giveResourceCommand },
      { "giveall", giveAllResourceCommand }
    };

    return COMMAND_LIST;
  }
}
}
