#include <filesystem>
#include <fstream>
#include <sstream>
#include "../Game/src/core/debug/debug_log.h"
#include "../Game/src/core/utility/general_utility.h"
#include "../Game/src/core/utility/string_utility.h"
#include "../Game/src/core/define.h"
#include "basearchive.h"
#include "basearchive_def.h"

namespace gengine2d
{
namespace
{
  int calculateVersionValue(std::int_least8_t V1, std::int_least8_t V2, std::int_least8_t V3, std::int_least8_t V4)
  {
    return (V1 * 1000) + (V2 * 100) + (V3 * 10) + V4;
  }

  bool writeFilename(std::ostream& out, const string_t& filename)
  {
    if(filename.size() + 1 <= GEARC_FILENAME_LENGTH) {
      out.write(filename.c_str(), filename.size());
      out.write(&NULL_CHAR, 1);

      auto spaces = string_t(GEARC_FILENAME_LENGTH - filename.size() - 1, ' ');
      out.write(spaces.c_str(), spaces.size());

      return true;
    }

    return false;
  }

  string_t getFilename(const char* buffStart, const char* buffEnd)
  {
    string_t ret;

    auto distance = std::distance(buffStart, buffEnd);
    if(distance > GEARC_FILENAME_LENGTH) {
      return "";
    }

    auto* it = buffStart;
    while(it != buffEnd) {
      auto c = *(it++);
      if(c == '\0')
        break;

      ret.push_back(c);
    }

    return ret;
  }

  void removeDirectoryEndingSlash(string_t& directory)
  {
    if(!directory.empty() && (directory.back() == '/' || directory.back() == '\\')) {
      directory.pop_back();
    }
  }

  void tryResetIStreamBits(std::istream* stream)
  {
    if(stream->fail() && stream->eof()) {
      auto state = stream->rdstate();
      state &= ~std::ios_base::failbit;
      state &= ~std::ios_base::eofbit;
      stream->clear(state);
    }
  }
}
#pragma pack(push, 1)
  struct ArchiveHeader
  {
    std::int_least8_t archive[2];
    std::int_least8_t version[4];
    std::int_least8_t magic[4];

    bool operator==(const ArchiveHeader& other)
    {
      for(std::size_t i = 0; i < 2; ++i) {
        if(archive[i] != other.archive[i]) {
          return false;
        }
      }

      for(std::size_t i = 0; i < 4; ++i) {
        if(magic[i] != other.magic[i]) {
          return false;
        }
      }

      return true;
    }

    bool operator!=(const ArchiveHeader& other)
    {
      return !(*this == other);
    }

    bool versionValid() const
    {
      for(std::size_t i = 0; i < 4; ++i) {
        if(version[i] > 9) {
          return false;
        }
      }

      return true;
    }

    int versionValue() const
    {
      return calculateVersionValue(version[0],
                                   version[1],
                                   version[2],
                                   version[3]);
    }

    static ArchiveHeader getDefault()
    {
      ArchiveHeader header;
      header.archive[0] = GEARC_ARCHIVE_01;
      header.archive[1] = GEARC_ARCHIVE_02;
      header.version[0] = GEARC_VERSION_01;
      header.version[1] = GEARC_VERSION_02;
      header.version[2] = GEARC_VERSION_03;
      header.version[3] = GEARC_VERSION_04;
      header.magic[0] = GEARC_MAGIC_01;
      header.magic[1] = GEARC_MAGIC_02;
      header.magic[2] = GEARC_MAGIC_03;
      header.magic[3] = GEARC_MAGIC_04;

      return header;
    }
  };
#pragma pack(pop)
  BaseArchiveFile::BaseArchiveFile(std::unique_ptr<std::stringstream> dataStream) :
    m_dataStream(std::move(dataStream))
  {
  }

  std::istream* BaseArchiveFile::getInputStream()
  {
    return m_dataStream.get();
  }

  BaseArchive::BaseArchive()
  {
    static_assert(sizeof(ArchiveHeader) == 10, "ArchiveHeader paddings was not removed!");
  }

  void BaseArchive::close()
  {
    m_folderTree.clear();
    m_fileTree.clear();
  }

  int BaseArchive::open(const string_t& filename)
  {
    std::ifstream in(filename, std::ios::binary);
    if(in.is_open()) {
      return open(in);
    }

    return EOR_INVALIDFILE;
  }

  int BaseArchive::open(std::istream& stream)
  {
    this->close();

    // read headers
    auto readHeaderRes = readHeaders(stream);
    if(readHeaderRes != EOR_SUCCESS) {
      return readHeaderRes;
    }

    // read folder list
    auto folderList = readFolderList(stream);
    if(!folderList.first) {
      return EOR_INVALIDARCHIVE;
    }

    // add the folders read to the folder tree
    for(auto folderName : folderList.second) {
      removeDirectoryEndingSlash(folderName);
      m_folderTree.emplace(folderName);
    }

    // read file list
    auto fileList = readFileList(stream);
    if(!fileList.first) {
      return EOR_INVALIDARCHIVE;
    }

    // read files
    for(const auto& [fileName, fileSize] : fileList.second) {
      std::filesystem::path filePath(fileName);
      auto parentPath = filePath.parent_path().string();
      if(!parentPath.empty() && m_folderTree.find(parentPath) == m_folderTree.end()) {
        return EOR_INVALIDDIR;
      }

//      auto fileStream = std::make_unique<std::stringstream>(std::ios_base::in |
//                                                            std::ios_base::out |
//                                                            std::ios::binary);

//      array_t<char, GEARC_BUFF_LENGTH> buff = { 0 };
//      std::streamsize totalRead = 0;

//      while(totalRead < fileSize) {
//        auto diff = fileSize - totalRead;

//        stream.read(buff.data(), std::min<std::streamsize>(diff, buff.size()));
//        auto read = stream.gcount();

//        if(read) {
//          totalRead += read;
//          fileStream->write(buff.data(), read);
//        } else {
//          m_fileTree.clear();
//          return EOR_READERROR;
//        }
//      }
      auto [fileStream, totalRead] = readToFileStream(stream, fileSize);
      if(!fileStream) {
        close();
        return EOR_READERROR;
      }

      BaseArchiveFile file(std::move(fileStream));
      m_fileTree.emplace(fileName,
                         std::make_pair(std::make_shared<BaseArchiveFile>(std::move(file)), fileSize));
    }

    return EOR_SUCCESS;
  }

  int BaseArchive::save(const string_t& filename)
  {
    std::ofstream out(filename, std::ios::binary);
    if(out.is_open()) {
      return save(out);
    }

    return ESR_INVALIDFILE;
  }

  int BaseArchive::save(std::ostream& stream)
  {
    // write header
    ArchiveHeader header = ArchiveHeader::getDefault();
    stream.write(general_utils::static_cast_to_ptr_type<char>(&header), sizeof(header));

    // write folder list
    std::int_least64_t folderListSize = m_folderTree.size();
    stream.write(general_utils::static_cast_to_ptr_type<char>(&folderListSize), sizeof(std::int_least64_t));
    for(const auto& folder : m_folderTree) {
      if(!writeFilename(stream, folder)) {
        return ESR_WRITEERROR_FOLDER;
      }
    }

    // write file list
    std::int_least64_t fileListSize = m_fileTree.size();
    stream.write(general_utils::static_cast_to_ptr_type<char>(&fileListSize), sizeof(std::int_least64_t));
    for(const auto& file : m_fileTree) {
      if(!writeFilename(stream, file.first)) {
        return ESR_WRITEERROR_FILENAME;
      }
      stream.write(general_utils::static_cast_to_ptr_type<char>(&file.second.second), sizeof(std::int_least64_t));
    }

    // write each files
    for(const auto& file : m_fileTree) {
      auto* fileStream = file.second.first->getInputStream();
      auto fileSize = file.second.second;

      if(!writeFromFileStream(fileStream, fileSize, stream)) {
        return ESR_WRITEERROR_DATA;
      }
//      array_t<char, GEARC_BUFF_LENGTH> buff = { 0 };
//      std::streamsize totalWrite = 0;

//      auto* fileStream = file.second.first->getInputStream();
//      fileStream->seekg(0, std::ios_base::beg);

//      while(totalWrite < file.second.second) {
//        fileStream->read(buff.data(), buff.size());
//        auto read = fileStream->gcount();

//        if(read) {
//          totalWrite += read;
//          stream.write(buff.data(), read);
//        } else {
//          DEBUG_LOG_ERR("Total Write: ", totalWrite, " Needed: ", file.second.second);
//          return ESR_WRITEERROR_DATA;
//        }
//      }

//      tryResetIStreamBits(fileStream);

//      // reset the input streams of each file
//      fileStream->seekg(0, std::ios_base::beg);
    }

    return ESR_SUCCESS;
  }

  bool BaseArchive::makeDir(const string_t& directoryName)
  {
    std::filesystem::path directoryPath(directoryName);
    auto actualPath = directoryPath.parent_path().string();
    removeDirectoryEndingSlash(actualPath);

    auto actualDirectoryName = directoryName;
    removeDirectoryEndingSlash(actualDirectoryName);

    if(!actualPath.empty() && m_folderTree.find(actualPath) == m_folderTree.end()) {
      DEBUG_LOG_ERR("Parent folder \"" + actualPath + "\" doesn't exists!");
      return false;
    }

    if(dirExists(actualDirectoryName)) {
      DEBUG_LOG_ERR("Folder with the same name already exists!");
      return false;
    }

    if(fileExists(actualDirectoryName)) {
      DEBUG_LOG_ERR("File with the same name already exists!");
      return false;
    }

    m_folderTree.emplace(directoryName);
    return true;
  }

  bool BaseArchive::removeDir(const string_t& directoryName)
  {
    if(directoryName.empty()) {
      DEBUG_LOG_ERR("Deleting empty name");
      return false;
    }

    auto actualDirectoryName = directoryName;
    removeDirectoryEndingSlash(actualDirectoryName);

    if(auto findIt = m_folderTree.find(actualDirectoryName); findIt != m_folderTree.end()) {
      m_folderTree.erase(findIt);

      for(auto it = m_fileTree.begin(), itEnd = m_fileTree.end(); it != itEnd; ++it) {
        const auto& [filename, fileStream] = *it;

        auto actualDirectoryNameWithSlash = actualDirectoryName + "/";
        if(matchBeginning(actualDirectoryNameWithSlash, filename)) {
          m_fileTree.erase(it);
        }
      }

      return true;
    }

    DEBUG_LOG_ERR("Folder \"", directoryName, "\" doesn't exist!");
    return false;
  }

  bool BaseArchive::addFile(const string_t& filename, const string_t& archiveDirectory)
  {
    std::ifstream in(filename, std::ios::binary);
    if(in.is_open()) {
      return addFile(in, filename, archiveDirectory);
    }

    DEBUG_LOG_ERR("Cannot open file \"" + filename + "\"");
    return false;
  }

  bool BaseArchive::addFile(std::istream& stream,
                            const string_t& filename,
                            const string_t& archiveDirectory)
  {
    std::filesystem::path filePath(filename);
    auto actualFilename = filePath.filename().string();
    auto actualPath = archiveDirectory;
    removeDirectoryEndingSlash(actualPath);

    // check if folder exists
    if(!actualPath.empty() && m_folderTree.find(actualPath) == m_folderTree.end()) {
      DEBUG_LOG_ERR("Parent folder \"" + actualPath + "\" doesn't exists!");
      return false;
    }

    auto slash = actualPath.empty() ? string_t("") : string_t("/");
    actualFilename = actualPath + slash + actualFilename;

    // check filename length
    if(actualFilename.size() >= GEARC_FILENAME_LENGTH) {
      DEBUG_LOG_ERR("Filename exceeds maximum filename length");
      return false;
    }

    // check if the filename is already an existing directory
    if(dirExists(actualFilename)) {
      DEBUG_LOG_ERR("Folder with the same name already exists!");
      return false;
    }

    if(fileExists(actualFilename)) {
      DEBUG_LOG_ERR("File with the same name already exists!");
      return false;
    }

//    auto fileStream = std::make_unique<std::stringstream>(std::ios_base::in |
//                                                          std::ios_base::out |
//                                                          std::ios::binary);

//    array_t<char, GEARC_BUFF_LENGTH> buff = { 0 };
//    std::streamsize totalRead = 0;

//    stream.read(buff.data(), buff.size());
//    auto read = stream.gcount();

//    while(read) {
//      fileStream->write(&buff[0], read);
//      totalRead += read;

//      stream.read(buff.data(), buff.size());
//      read = stream.gcount();
//    }
    auto [fileStream, totalRead] = readToFileStream(stream);

    BaseArchiveFile file(std::move(fileStream));
    m_fileTree.emplace(actualFilename,
                       std::make_pair(std::make_shared<BaseArchiveFile>(std::move(file)), totalRead));

    return true;
  }

  bool BaseArchive::removeFile(const string_t& filename)
  {
    if(auto findIt = m_fileTree.find(filename); findIt != m_fileTree.end()) {
      m_fileTree.erase(findIt);
      return true;
    }

    DEBUG_LOG_ERR("File \"", filename, "\" doesn't exist!");
    return false;
  }

  bool BaseArchive::extractTo(const string_t& filename, const string_t& outputDir) const
  {
    auto actualOutputDir = outputDir;
    removeDirectoryEndingSlash(actualOutputDir);

    auto outputFilename = std::filesystem::path(actualOutputDir) / std::filesystem::path(filename);
    return extractFile(filename, outputFilename.string());
  }

  bool BaseArchive::extractFile(const string_t& filename, const string_t& outputFilename) const
  {
    std::filesystem::path outputPath(outputFilename);
    auto actualOutputPath = outputPath.parent_path().string();
    if(!std::filesystem::exists(actualOutputPath)) {
      std::filesystem::create_directory(actualOutputPath);
    }

    std::ofstream out(outputFilename, std::ios::binary);
    if(out.is_open()) {
      return extractFile(filename, out);
    }

    DEBUG_LOG_ERR("Cannot create file \"" + outputFilename + "\"");
    return false;
  }

  bool BaseArchive::extractFile(const string_t& filename, std::ostream& stream) const
  {
    auto findIt = m_fileTree.find(filename);
    if(findIt != m_fileTree.end()) {
      auto* fileStream = findIt->second.first->getInputStream();
      auto fileSize = findIt->second.second;

      return writeFromFileStream(fileStream, fileSize, stream);
//      fileStream->seekg(0, std::ios_base::beg);

//      array_t<char, GEARC_BUFF_LENGTH> buff = { 0 };
//      fileStream->read(buff.data(), buff.size());
//      auto read = fileStream->gcount();
//      while(read) {
//        stream.write(buff.data(), read);

//        fileStream->read(buff.data(), buff.size());
//        read = fileStream->gcount();
//      }

//      tryResetIStreamBits(fileStream);

//      // reset the input streams of file
//      fileStream->seekg(0, std::ios_base::beg);

//      return true;
    }

    DEBUG_LOG_ERR("File \"", filename, "\" doesnt exist!");
    return false;
  }

  bool BaseArchive::extractAll(const string_t& outputDirectory) const
  {
    auto actualPath = outputDirectory;
    if(actualPath.empty()) {
      DEBUG_LOG_ERR("Empty directory path");
      return false;
    }

    removeDirectoryEndingSlash(actualPath);

    auto fileList = this->getFileList();
    for(const auto& filename : fileList) {
      auto outputPath = std::filesystem::path(actualPath) / std::filesystem::path(filename);
      if(!extractFile(filename, outputPath.string())) {
        return false;
      }
    }

    return true;
  }

  bool BaseArchive::moveFile(const string_t& filename, const string_t& newFilename)
  {
    auto newFilenamePath = std::filesystem::path(newFilename).parent_path().string();
    removeDirectoryEndingSlash(newFilenamePath);

    if(newFilenamePath == "" || m_folderTree.find(newFilenamePath) != m_folderTree.end()) {
      if(auto findIt = m_fileTree.find(filename); findIt != m_fileTree.end()) {
        auto fileStream = std::move(findIt->second);
        m_fileTree.erase(findIt);
        m_fileTree.emplace(newFilename, std::move(fileStream));
        return true;
      }
    }

    return false;
  }

  BaseArchiveFile* BaseArchive::getFile(const string_t& filename)
  {
    auto findIt = m_fileTree.find(filename);
    if(findIt != m_fileTree.end()) {
      return findIt->second.first.get();
    }

    return nullptr;
  }

  vector_t<string_t> BaseArchive::getFileList() const
  {
    vector_t<string_t> ret;
    for(const auto& [filename, fileStream] : m_fileTree) {
      ret.push_back(filename);
    }

    return ret;
  }

  vector_t<string_t> BaseArchive::getDirList() const
  {
    vector_t<string_t> ret;
    for(const auto& dirname : m_folderTree) {
      ret.push_back(dirname);
    }

    return ret;
  }

  bool BaseArchive::fileExists(const string_t& filename) const
  {
    return m_fileTree.find(filename) != m_fileTree.end();
  }

  bool BaseArchive::dirExists(const string_t& directoryName) const
  {
    return m_folderTree.find(directoryName) != m_folderTree.end();
  }

  BaseArchive::EOpenResult BaseArchive::readHeaders(std::istream& stream)
  {
    ArchiveHeader header;
    stream.read(general_utils::static_cast_to_ptr_type<char>(&header), sizeof(header));
    auto read = stream.gcount();
    if(read != sizeof(header)) {
      return EOR_READERROR;
    }

    if(header != ArchiveHeader::getDefault()) {
      return EOR_INVALIDHEADERS;
//      return false;
    }

    if(!header.versionValid()) {
      return EOR_INVALIDHEADERVER;
//      return false;
    }

    auto versionValue = header.versionValue();
    auto minVersionValue = calculateVersionValue(GEARC_VERSION_MIN_01,
                                                 GEARC_VERSION_MIN_02,
                                                 GEARC_VERSION_MIN_03,
                                                 GEARC_VERSION_MIN_04);
    auto maxVersionValue = calculateVersionValue(GEARC_VERSION_01,
                                                 GEARC_VERSION_02,
                                                 GEARC_VERSION_03,
                                                 GEARC_VERSION_04);

    if(versionValue < minVersionValue || versionValue > maxVersionValue) {
      return EOR_INCOMPATIBLEVER;
    }

    return EOR_SUCCESS;
  }

  std::pair<bool, vector_t<string_t>> BaseArchive::readFolderList(std::istream& stream)
  {
    vector_t<string_t> ret;

    array_t<char, GEARC_BUFF_LENGTH> buff = { 0 };

    // read folder count
    stream.read(buff.data(), sizeof(std::int_least64_t));
    if(stream.gcount()) {
      std::int_least64_t numberOfFolders = *general_utils::static_cast_to_ptr_type<std::int_least64_t>(buff.data());
      for(std::int_least64_t i = 0; i < numberOfFolders; ++i) {
        // read folder name
        stream.read(buff.data(), GEARC_FILENAME_LENGTH);
        auto read = stream.gcount();
        if(!read) {
          return {};
        }
        auto folderName = getFilename(buff.data(), buff.data() + read);
        if(folderName.empty()) {
          return { false, {} };
        }

        ret.push_back(folderName);
      }

      return { true, ret };
    }

    return { false, {} };
  }

  std::pair<bool, vector_t<std::pair<string_t, std::int_least64_t>>> BaseArchive::readFileList(std::istream& stream)
  {
    vector_t<std::pair<string_t, std::int_least64_t>> ret;

    array_t<char, GEARC_BUFF_LENGTH> buff = { 0 };

    // read file count
    stream.read(buff.data(), sizeof(std::int_least64_t));
    if(stream.gcount()) {
      std::int_least64_t numberOfFiles = *general_utils::static_cast_to_ptr_type<std::int_least64_t>(buff.data());
      for(std::int_least64_t i = 0; i < numberOfFiles; ++i) {
        // read filename
        stream.read(buff.data(), GEARC_FILENAME_LENGTH);
        auto read = stream.gcount();
        if(!read) {
          return { false, {} };
        }
        auto filename = getFilename(buff.data(), buff.data() + read);
        if(filename.empty()) {
          return { false, {} };
        }

        // read file size
        stream.read(buff.data(), sizeof(std::int_least64_t));
        if(!stream.gcount()) {
          return { false, {} };
        }
        std::int_least64_t fileSize = *general_utils::static_cast_to_ptr_type<std::int_least64_t>(buff.data());
        if(!fileSize) {
          return { false, {} };
        }

        ret.push_back(std::make_pair(filename, fileSize));
      }

      return { true, ret };
    }

    return { false, {} };
  }

  std::pair<std::unique_ptr<std::stringstream>, std::streamsize> BaseArchive::readToFileStream(std::istream& stream)
  {
    auto fileStream = std::make_unique<std::stringstream>(std::ios_base::in |
                                                          std::ios_base::out |
                                                          std::ios::binary);

    array_t<char, GEARC_BUFF_LENGTH> buff = { 0 };
    std::streamsize totalRead = 0;

    stream.read(buff.data(), buff.size());
    auto read = stream.gcount();

    while(read) {
      fileStream->write(&buff[0], read);
      totalRead += read;

      stream.read(buff.data(), buff.size());
      read = stream.gcount();
    }

    return std::make_pair(std::move(fileStream), totalRead);
  }

  std::pair<std::unique_ptr<std::stringstream>, std::streamsize> BaseArchive::readToFileStream(std::istream& stream, std::streamsize sizeToRead)
  {
    auto fileStream = std::make_unique<std::stringstream>(std::ios_base::in |
                                                          std::ios_base::out |
                                                          std::ios::binary);
    array_t<char, GEARC_BUFF_LENGTH> buff = { 0 };
    std::streamsize totalRead = 0;

    while(totalRead < sizeToRead) {
      auto diff = sizeToRead - totalRead;

      stream.read(buff.data(), general_utils::min<std::streamsize>(diff, buff.size()));
      auto read = stream.gcount();

      if(read) {
        totalRead += read;
        fileStream->write(buff.data(), read);
      } else {
        return std::make_pair(std::unique_ptr<std::stringstream>(nullptr), totalRead);
      }
    }

    return std::make_pair(std::move(fileStream), totalRead);
  }

  bool BaseArchive::writeFromFileStream(std::istream* fileStream, std::streamsize fileSize, std::ostream& stream) const
  {
    fileStream->seekg(0, std::ios_base::beg);

    array_t<char, GEARC_BUFF_LENGTH> buff = { 0 };
    std::streamsize totalWrite = 0;

    while(totalWrite < fileSize) {
      fileStream->read(buff.data(), buff.size());
      auto read = fileStream->gcount();

      if(read) {
        totalWrite += read;
        stream.write(buff.data(), read);
      } else {
        tryResetIStreamBits(fileStream);
        fileStream->seekg(0, std::ios_base::beg);
        return false;
      }
    }

    tryResetIStreamBits(fileStream);
    fileStream->seekg(0, std::ios_base::beg);

    return true;
  }
}
