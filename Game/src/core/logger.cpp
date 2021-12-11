#include "logger.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include "define.h"

namespace gengine2d
{
  void Logger::write(string_t msg)
  {
    gengine2d::string_t logFilename = LOG_FILENAME;
    auto logParentPath = std::filesystem::path(LOG_FILENAME).parent_path();
    if(!std::filesystem::exists(logParentPath)) {
      if(!std::filesystem::create_directory(logParentPath)) {
        logFilename = std::filesystem::path(LOG_FILENAME).filename();
      }
    }

    std::ofstream out(logFilename, std::ios::app);
    if(out.is_open()) {
      auto currentCTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
      out << ctime(&currentCTime) << ": " << msg << std::endl;
    }
  }

  void Logger::writeError(string_t msg)
  {
    write("[ERROR]: " + std::move(msg));
  }

  void Logger::writeWarning(string_t msg)
  {
    write("[Warning]: " + std::move(msg));
  }
}
