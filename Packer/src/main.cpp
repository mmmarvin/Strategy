#include <iostream>
#include <filesystem>
#include "../../Game/src/core/utility/string_utility.h"
#include "../../Game/src/core/archive_type.h"
#include "../../Game/src/core/type.h"
#include "commands.h"

bool checkValidUsage(std::size_t tokensSize, std::size_t requiredSize, const gengine2d::string_t& cmd)
{
  if(tokensSize == requiredSize) {
    return true;
  }

  printInvalidUsage(cmd);
  return false;
}

int runCommandsOnArchive(gengine2d::archive_t& archive)
{
  gengine2d::string_t currentDir = "";
  gengine2d::string_t command;

  for(;;) {
    std::cout << ">";
    std::getline(std::cin, command);

    auto tokens = gengine2d::tokenize2(command, ' ');
    if(tokens.size() > 0) {
      auto cmd = tokens[0];

      if(cmd == "cd") {
        if(checkValidUsage(tokens.size(), 2, cmd)) {
          perfChangeDir(archive, tokens[1], currentDir);
        }

      } else if(cmd == "mkdir") {
        if(checkValidUsage(tokens.size(), 2, cmd)) {
          perfMakeDir(archive, tokens[1], currentDir);
        }
      } else if(cmd == "pwd") {
        if(checkValidUsage(tokens.size(), 1, cmd)) {
          std::cout << currentDir << std::endl;
        }
      } else if(cmd == "exit") {
        break;
      } else if(cmd == "ls") {
        auto fileList = archive.getFileList();
        auto folderList = archive.getDirList();

        if(tokens.size() == 1) {
          perfListDir(folderList, currentDir);
          perfListFile(fileList, currentDir);
        } else if(tokens.size() == 2) {
          auto param = tokens[1];
          if(param == "-d") {
            perfListDir(folderList, currentDir);
          } else if(param == "-dn") {
            perfListDir(folderList, currentDir, false);
          } else if(param == "-dc") {
            perfListDirCount(folderList, currentDir);
          } else if(param == "-f") {
            perfListFile(fileList, currentDir);
          } else if(param == "-fc") {
            perfListFileCount(fileList, currentDir);
          } else if(param == "-all") {
            for(const auto& filename : fileList) {
              std::cout << filename << std::endl;
            }
          } else if(param == "-al") {
            std::cout << "Filename\tDate Created\tDate Modified\n";
            perfListDir(folderList, currentDir);
            perfListFileDetail(archive, fileList, currentDir);
          }
        } else {
          printInvalidUsage(cmd);
        }
      } else if(cmd == "add") {
        if(checkValidUsage(tokens.size(), 2, cmd)) {
          perfAdd(archive, tokens[1], currentDir);
        }
      } else if(cmd == "rm") {
        if(checkValidUsage(tokens.size(), 2, cmd)) {
          perfRemove(archive, tokens[1], currentDir);
        }
      } else if(cmd == "extract") {
        if(tokens.size() == 3) {
          perfExtract(archive, tokens[1], currentDir, tokens[2]);
        } else if(tokens.size() == 2) {
          auto outputPath = std::filesystem::path(tokens[1]).parent_path().string();
          if(outputPath.empty())
            outputPath = ".";
          perfExtract(archive, tokens[1], currentDir, outputPath);
        } else {
          printInvalidUsage(cmd);
        }
      } else if(cmd == "extractall") {
        if(checkValidUsage(tokens.size(), 2, cmd)) {
          perfExtractAll(archive, tokens[1]);
        }
      } else if(cmd == "save") {
        if(checkValidUsage(tokens.size(), 2, cmd)) {
          perfSave(archive, tokens[1]);
        }
      } else {
        std::cout << "Unknown command \"" + cmd + "\"" << std::endl;
      }
    }
  }

  return 0;
}

int main(int argc, char** argv)
{
  gengine2d::archive_t archive;

  if(argc == 2) {
    gengine2d::string_t argvs(argv[1]);
    if(argvs == "--help") {
      printHelp(argv[0]);
    } else if(argvs == "--new") {
      return runCommandsOnArchive(archive);
    } else {
      if(auto openRes = archive.open(argvs); openRes != gengine2d::archive_t::EOR_SUCCESS) {
        std::cout << "Cannot open archive! Error code: " << std::hex << openRes << std::endl;
        return 1;
      } else {
        return runCommandsOnArchive(archive);
      }
    }
  } else {
    printHelp(argv[0]);
  }

  return 0;
}
