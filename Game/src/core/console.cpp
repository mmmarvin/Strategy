#include "console.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include "debug/debug_log.h"
#include "utility/container_utility.h"
#include "utility/string_utility.h"
#include "game.h"

namespace gengine2d
{
namespace
{
  void removeNewspaceInMessage(string_t& msg)
  {
    auto it = remove_if(msg.begin(), msg.end(), [](auto c) {
        return c == '\n';
    });
    if(it != msg.end())
      msg.erase(it, msg.end());
  }
}
  Console::Console(std::size_t maxMessages,
                   std::size_t maxCmdHistory) :
    m_maxMessages(maxMessages),
    m_maxCmdHistory(maxCmdHistory)
  {
    m_cmdHistoryPointer = m_cmdHistory.begin();
  }

  void Console::draw()
  {
    string_t msgs;
    for(const auto& msg : m_messages) {
      msgs += msg;
      msgs.push_back('\n');
    }

    const float footerHeight = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    string_t cmd;
    cmd.reserve(256);

    // TODO: Fix input text not scrolling to bottom of page when entering
    // a command
    if(!ImGui::IsPopupOpen("Console")) {
      ImGui::OpenPopup("Console");
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(500, 250));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.f);
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.f, 0.f, 0.f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(0.f, 0.f, 0.f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(0.f, 0.f, 0.f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(0.f, 0.f, 0.f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ResizeGrip, ImVec4(0.2f, 0.2f, 0.2f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ResizeGripActive, ImVec4(0.3f, 0.3f, 0.3f, 0.9f));
    ImGui::PushStyleColor(ImGuiCol_ResizeGripHovered, ImVec4(0.4f, 0.4f, 0.4f, 0.9f));
    if(ImGui::BeginPopupModal("Console", 0, ImGuiWindowFlags_NoCollapse)) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.f, 1.f, 1.f, 1.f));
      ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.f, 0.f, 0.f, 0.f));
      ImGui::BeginChild("##Console.Frame",
                        ImVec2(0, -footerHeight),
                        false,
                        ImGuiWindowFlags_NoScrollbar);
      ImGui::InputTextMultiline("##Console.Messages",
                                &msgs,
                                ImVec2(-1, -1),
                                ImGuiInputTextFlags_ReadOnly);
      ImGui::BeginChild(ImGui::GetID("##Console.Messages"));
      ImGui::SetScrollHereY(1.f);
      ImGui::EndChild();
      ImGui::EndChild();
      ImGui::Separator();
      ImGui::PushItemWidth(-1);
      if(ImGui::InputText("##Console.TextInput",
                          &cmd,
                          ImGuiInputTextFlags_EnterReturnsTrue |
                          ImGuiInputTextFlags_CallbackHistory |
                          ImGuiInputTextFlags_CallbackCompletion,
      [](ImGuiInputTextCallbackData* data) -> int {
        auto* console = static_cast<Console*>(data->UserData);
        auto event = data->EventFlag;
        if(event == ImGuiInputTextFlags_CallbackHistory) {
          if(!console->m_cmdHistory.empty()) {
            if(data->EventKey == ImGuiKey_UpArrow) {
              if(console->m_cmdHistoryPointer == console->m_cmdHistory.begin()) {
                console->m_cmdHistoryPointer = console->m_cmdHistory.end() - 1;
              } else {
                --console->m_cmdHistoryPointer;
              }

              data->DeleteChars(0, data->BufTextLen);
              data->InsertChars(0, console->m_cmdHistoryPointer->data());
            } else if(data->EventKey == ImGuiKey_DownArrow) {
              if(console->m_cmdHistoryPointer == console->m_cmdHistory.end()) {
                console->m_cmdHistoryPointer = console->m_cmdHistory.begin();
              } else {
                ++console->m_cmdHistoryPointer;
              }

              if(console->m_cmdHistoryPointer == console->m_cmdHistory.end()) {
                data->DeleteChars(0, data->BufTextLen);
              } else {
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, console->m_cmdHistoryPointer->data());
              }
            }
          }
        } else if(event == ImGuiInputTextFlags_CallbackCompletion) {
          const auto* word_end = data->Buf + data->CursorPos;
          const auto* word_start = word_end;
          while (word_start > data->Buf)
          {
              const auto c = word_start[-1];
              if (c == ' ' || c == '\t' || c == ',' || c == ';')
                  break;
              word_start--;
          }

          string_t partialCmd(word_start, word_start + (word_end - word_start));
          if(!partialCmd.empty()) {
            vector_t<string_t> candidates;
            for(const auto& command : GameEngine::getInstance().getCommandList()) {
              if(partialMatch(partialCmd, command)) {
                candidates.push_back(command);
              }
            }
            std::sort(candidates.begin(), candidates.end());

            console->printLine("Possible Matches: ");
            for(const auto& command : candidates) {
              console->printLine("- " + command);
            }
          }
        }

        return 0;
      }, static_cast<void*>(this))) {
        string_t nullTerminatedString(cmd.data());
        if(!nullTerminatedString.empty()) {
          m_messages.push_back("# " + nullTerminatedString);
          GameEngine::getInstance().apply(nullTerminatedString);
          m_cmdHistory.push_back(nullTerminatedString);
          container_utils::shrinkFrontToFitSize(m_messages, m_maxMessages);
          m_cmdHistoryPointer = m_cmdHistory.end() - 1;
        }
      }
      if (ImGui::IsItemHovered() ||
          (ImGui::IsRootWindowOrAnyChildFocused() &&
           !ImGui::IsAnyItemActive() &&
           !ImGui::IsMouseClicked(0))) {
          ImGui::SetKeyboardFocusHere(-1);
      }
      ImGui::PopItemWidth();
      ImGui::PopStyleColor(2);
      ImGui::EndPopup();
    }
    ImGui::PopStyleColor(7);
    ImGui::PopStyleVar(2);
  }

  // TODO: Remove this?
  void Console::handleInput(const InputEvent&)
  {
  }

  void Console::clear()
  {
    m_messages.clear();
  }

  void Console::print(string_t msg)
  {
    if(m_messages.empty()) {
      printLine(std::move(msg));
    } else {
      DEBUG_LOG("Console: ", msg);
      auto& lastMessage = m_messages.back();

      removeNewspaceInMessage(msg);

      lastMessage.push_back(' ');
      lastMessage.insert(lastMessage.end(), msg.begin(), msg.end());
    }
  }

  void Console::printLine(string_t msg)
  {
    DEBUG_LOG("Console: ", msg);
    removeNewspaceInMessage(msg);

    m_messages.push_back(move(msg));
    container_utils::shrinkFrontToFitSize(m_messages, m_maxMessages);
  }

  void Console::printWarning(string_t msg)
  {
    printLine("Warning: " + move(msg));
  }

  void Console::printError(string_t msg)
  {
    printLine("ERROR: " + move(msg));
  }
}
