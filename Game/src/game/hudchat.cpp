#include "hudchat.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include "../core/game.h"
#include "../core/utility/container_utility.h"
#include "game_def.h"

namespace strat
{
  HUDChat::HUDChat() :
    m_maxMessages(30),
    m_messageMode(false)
  {
  }

  void HUDChat::draw()
  {
    const auto& renderer = gengine2d::GameEngine::getInstance().getRenderer();
    const auto startingPos = renderer.getScreenSize().y -
                             HUD_LOWER_HEIGHT -
                             HUD_CHAT_HEIGHT -
                             HUD_CHAT_INPUT_HEIGHT -
                             10;

    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.25f));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    if(m_messageMode ||
       (m_durationClock.is_running() && m_durationClock.get_elapsed().as_seconds() <= 5.f)) {
      ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.f));
      ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.f, 0.f, 0.f, 0.f));
      ImGui::PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0.f, 0.f, 0.f, 0.f));

      gengine2d::string_t msgs;
      for(const auto& [to, from, msg] : m_messages) {
        msgs += "(" + to + ") " + from + ": " + msg + "\n";
      }

      ImGui::SetNextWindowPos(ImVec2(10, startingPos));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(HUD_CHAT_WIDTH, HUD_CHAT_HEIGHT));
      ImGui::Begin("##UI.HUD.Chat",
                   0,
                   ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoScrollbar);
      ImGui::InputTextMultiline("##UI.HUD.Chat.Messages",
                                &msgs,
                                ImVec2(-1, -1),
                                ImGuiInputTextFlags_ReadOnly);
      ImGui::BeginChild(ImGui::GetID("##UI.HUD.Chat.Messages"));
      ImGui::SetScrollHereY(1.f);
      ImGui::EndChild();
      ImGui::End();
      ImGui::PopStyleVar(1);
      ImGui::PopStyleColor(3);

      if(m_durationClock.is_running() && m_durationClock.get_elapsed().as_seconds() > 5.f) {
        m_durationClock.stop();
      }
    }
    if(m_messageMode) {
      gengine2d::string_t cmd;

      ImGui::SetNextWindowPos(ImVec2(10, startingPos + 149));
      ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(HUD_CHAT_WIDTH, HUD_CHAT_INPUT_HEIGHT));
      ImGui::Begin("##UI.HUD.ChatInput",
                   0,
                   ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoMove |
                   ImGuiWindowFlags_NoResize);
      ImGui::PushItemWidth(-1);
      if(ImGui::InputText("##UI.HUD.ChatInput.Input",
                          &cmd,
                          ImGuiInputTextFlags_EnterReturnsTrue)) {
        gengine2d::string_t nullTerminatedString(cmd.data());
        if(!nullTerminatedString.empty()) {
          pushMessage("ALL", "name", nullTerminatedString);
        }
      }
      ImGui::PopItemWidth();
      if (ImGui::IsItemHovered() ||
          (ImGui::IsRootWindowOrAnyChildFocused() &&
           !ImGui::IsAnyItemActive() &&
           !ImGui::IsMouseClicked(0))) {
        ImGui::SetKeyboardFocusHere(-1);
      }
      ImGui::End();
      ImGui::PopStyleVar(1);
    }
    ImGui::PopStyleVar(1);
    ImGui::PopStyleColor(5);
  }

  void HUDChat::pushMessage(gengine2d::string_t to,
                            gengine2d::string_t from,
                            gengine2d::string_t message)
  {
    if(m_messages.size() >= m_maxMessages) {
      gengine2d::container_utils::shrinkFrontToFitSize(m_messages, m_maxMessages);
    }

    m_messages.push_back(std::make_tuple(std::move(to),
                                         std::move(from),
                                         std::move(message)));
    m_durationClock.restart();
  }

  void HUDChat::setMessageMode(bool mode)
  {
    m_messageMode = mode;
  }
}
