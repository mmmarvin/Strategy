#include <iostream>
#include <filesystem>
#include "../../Game/src/core/archive/basearchive_def.h"
#include "../../Game/src/core/debug/debug_log.h"
#include "../../Game/src/core/utility/config_utility.h"
#include "../../Game/src/core/utility/stl_utility.h"
#include "commands.h"
#include "config.h"

namespace
{
  gengine2d::string_t versionToString(int major1, int major2, int minor, int patch)
  {
    int major = (major1 * 10) + major2;
    return gengine2d::config_utils::versionToString(major, minor, patch);
  }

  template<class Func1,
           class Func2>
  void perfListDirImpl(const gengine2d::vector_t<gengine2d::string_t>& folderList,
                       const gengine2d::string_t& currentDir,
                       Func1&& func1,
                       Func2&& func2)
  {
    if(currentDir == "") {
      for(const auto& folder : folderList) {
        std::filesystem::path folderPath(folder);
        if(folderPath.parent_path().string() == "") {
          std::forward<Func1>(func1)(folder, folder);
        }
      }
    } else {
      std::forward<Func2>(func2)();
      for(const auto& folder : folderList) {
        std::filesystem::path folderPath(folder);
        if(folderPath.parent_path().string() == currentDir) {
          std::forward<Func1>(func1)(folder, folderPath.filename().string());
        }
      }
    }
  }

  template<class Func>
  void perfListFileImpl(const gengine2d::vector_t<gengine2d::string_t>& fileList,
                        const gengine2d::string_t& currentDir,
                        Func&& func)
  {
    if(currentDir == "") {
      for(const auto& file : fileList) {
        std::filesystem::path filePath(file);
        if(filePath.parent_path().string() == "") {
          std::forward<Func>(func)(file, file);
//          std::cout << file << std::endl;
        }
      }
    } else {
      for(const auto& file : fileList) {
        std::filesystem::path filePath(file);
        if(filePath.parent_path().string() == currentDir) {
          std::forward<Func>(func)(file, filePath.filename().string());
//          std::cout << filePath.filename().string() << std::endl;
        }
      }
    }
  }
}

void perfChangeDir(const gengine2d::archive_t& archive,
                   const gengine2d::string_t& dir,
                   gengine2d::string_t& currentDir)
{
  if(dir == "..") {
    currentDir = std::filesystem::path(currentDir).parent_path().string();
    printSuccess();
  } else {
    auto slash = currentDir.empty() ? gengine2d::string_t("") : gengine2d::string_t("/");
    if(!archive.dirExists(currentDir + slash + dir)) {
      std::cout << "Folder does not exists!" << std::endl;
    } else {
      currentDir = currentDir + slash + dir;
      printSuccess();
    }
  }
}

void perfMakeDir(gengine2d::archive_t& archive,
                 const gengine2d::string_t& dir,
                 const gengine2d::string_t& currentDir)
{
  auto slash = currentDir.empty() ? gengine2d::string_t("") : gengine2d::string_t("/");
  if(!archive.makeDir(currentDir + slash + dir)) {
    std::cout << "Cannot make dir \"" + dir + "\"!" << std::endl;
  } else {
    printSuccess();
  }
}

void perfListDir(const gengine2d::vector_t<gengine2d::string_t>& folderList,
                 const gengine2d::string_t& currentDir,
                 bool colored)
{
  auto nonColoredLister = [](const gengine2d::string_t&, const gengine2d::string_t& folder)
  {
    std::cout << folder << "\n";
  };

  auto coloredLister = [](const gengine2d::string_t&, const gengine2d::string_t& folder)
  {
    std::cout << "\x1B[94m";
    std::cout << folder << "\n";
    std::cout << "\033[0m";
  };

  auto prevNonColoredLister = [] {
    std::cout << ".." << "\n";
  };

  auto prevColoredLister = [] {
    std::cout << "\x1B[94m";
    std::cout << ".." << "\n";
    std::cout << "\033[0m";
  };

  auto lister = colored ? coloredLister : nonColoredLister;
  auto prevLister = colored ? prevColoredLister : prevNonColoredLister;
  perfListDirImpl(folderList, currentDir, lister, prevLister);
  std::cout.flush();
}

void perfListDirCount(const gengine2d::vector_t<gengine2d::string_t>& folderList,
                      const gengine2d::string_t& currentDir)
{
  int count = 0;
  perfListDirImpl(folderList, currentDir,
  [&count](const gengine2d::string_t&, const gengine2d::string_t&) {
    ++count;
  }, [] {});

  if(currentDir != "") {
    ++count;
  }
  std::cout << count << std::endl;
}

void perfListFile(const gengine2d::vector_t<gengine2d::string_t>& fileList,
                  const gengine2d::string_t& currentDir)
{
  perfListFileImpl(fileList, currentDir,
  [](const gengine2d::string_t&, const gengine2d::string_t& filename) {
    std::cout << filename << "\n";
  });
  std::cout.flush();
}

void perfListFileCount(const gengine2d::vector_t<gengine2d::string_t>& fileList,
                       const gengine2d::string_t& currentDir)
{
  int count = 0;
  perfListFileImpl(fileList, currentDir,
  [&count](const gengine2d::string_t&, const gengine2d::string_t&) {
    ++count;
  });

  std::cout << count << std::endl;
}

void perfListFileDetail(const gengine2d::archive_t& archive,
                        const gengine2d::vector_t<gengine2d::string_t>& fileList,
                        const gengine2d::string_t& currentDir)
{
  perfListFileImpl(fileList, currentDir,
  [&archive](const gengine2d::string_t& fullFilePath, const gengine2d::string_t& filename) {
    const auto& fileDetail = archive.getFile(fullFilePath)->getDetails();
    auto timeCreated = std::put_time(std::localtime(&fileDetail.timeCreated), "%m\\%d\\%y");
    auto timeModified = std::put_time(std::localtime(&fileDetail.timeModified), "%m\\%d\\%y");

    std::cout << filename << "\t" << timeCreated << "\t" << timeModified << "\n";
  });
  std::cout.flush();
}

void perfAdd(gengine2d::archive_t& archive,
             const gengine2d::string_t& filename,
             const gengine2d::string_t& currentDir)
{
  if(archive.addFile(filename, currentDir) != gengine2d::archive_t::EAR_SUCCESS) {
    std::cout << "Cannot add file \"" + filename + "\" to archive!" << std::endl;
  } else {
    printSuccess();
  }
}

void perfRemove(gengine2d::archive_t& archive,
                const gengine2d::string_t& filename,
                const gengine2d::string_t& currentDir)
{
  auto slash = currentDir.empty() ? gengine2d::string_t("") : gengine2d::string_t("/");
  auto actualFilename = currentDir + slash + filename;

  if(archive.dirExists(actualFilename)) {
    if(!archive.removeDir(actualFilename)) {
      std::cout << "Cannot remove folder \"" + actualFilename + "\"!" << std::endl;
    } else {
      printSuccess();
    }
  } else {
    if(!archive.removeFile(actualFilename)) {
      std::cout << "Cannot remove file \"" + actualFilename + "\"!" << std::endl;
    } else {
      printSuccess();
    }
  }
}

void perfExtract(gengine2d::archive_t& archive,
                 const gengine2d::string_t& filename,
                 const gengine2d::string_t& currentDir,
                 gengine2d::string_t outputDir)
{
  if(outputDir.empty()) {
    std::cout << "Invalid extract destination" << std::endl;
  }

  if(outputDir.back() == '/' || outputDir.back() == '\\') {
    outputDir.pop_back();
  }

  if(outputDir == ".") {
    outputDir = "";
  }

  auto actualOutputDir = outputDir;
  if(!actualOutputDir.empty()) {
    actualOutputDir += "/";
  }

  DEBUG_LOG(actualOutputDir + filename);

  auto slash = currentDir.empty() ? gengine2d::string_t("") : gengine2d::string_t("/");
  auto actualFilename = currentDir + slash + filename;
  if(!archive.extractFile(actualFilename, actualOutputDir + filename)) {
    std::cout << "Cannot extract file \"" + filename + "\" to \"" + outputDir + "\"" << std::endl;
  } else {
    printSuccess();
  }
}

void perfExtractAll(gengine2d::archive_t& archive,
                    gengine2d::string_t outputDir)
{
  if(outputDir.empty()) {
    std::cout << "Invalid extract destination" << std::endl;
  }

  if(outputDir.size() > 1 && (outputDir.back() == '/' || outputDir.back() == '\\')) {
    outputDir.pop_back();
  }

  if(!archive.extractAll(outputDir)) {
    std::cout << "Cannot extract files to \"" + outputDir + "\"" << std::endl;
  } else {
    printSuccess();
  }
}

void perfSave(gengine2d::archive_t& archive,
              const gengine2d::string_t& filename)
{
  auto archiveSaveRes = archive.save(filename);
  if(archiveSaveRes != gengine2d::archive_t::ESR_SUCCESS) {
    std::cout << "Cannot save archive to \"" << filename << "\"! Error code: " << std::hex << archiveSaveRes << std::endl;
  } else {
    printSuccess();
  }
}

void printHelp(const gengine2d::string_t program)
{
  std::filesystem::path programPath(program);

  std::cout << "Packer v." + gengine2d::config_utils::versionToString(PACKER_VER_MAJOR,
                                                                      PACKER_VER_MINOR,
                                                                      PACKER_VER_PATCH)
            << " using " << "gengine_pack v." << versionToString(gengine2d::GEARC_VERSION_01,
                                                                 gengine2d::GEARC_VERSION_02,
                                                                 gengine2d::GEARC_VERSION_03,
                                                                 gengine2d::GEARC_VERSION_04) << "\n\n"
            << "Usage: " << programPath.filename().string() << " [pack filename]" << std::endl;
}

void printInvalidUsage(const gengine2d::string_t& cmd)
{
  std::cout << "Invalid usage of \"" + cmd + "\"" << std::endl;
}

void printSuccess()
{
  std::cout << std::endl;
}
