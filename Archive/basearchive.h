#ifndef BASEARCHIVE_H
#define BASEARCHIVE_H

#include <memory>
#include <sstream>
#include "../Game/src/core/type.h"

namespace gengine2d
{
  class BaseArchive;
  class BaseArchiveFile
  {
  protected:
    BaseArchiveFile(std::unique_ptr<std::stringstream> dataStream);

  public:
    std::istream* getInputStream();

  private:
    friend class BaseArchive;

    std::unique_ptr<std::stringstream> m_dataStream;
  };

  class BaseArchive
  {
  public:
    enum EOpenResult : int
    {
      EOR_SUCCESS = 0,
      EOR_INVALIDFILE,      // ErrorCode 1
      EOR_INVALIDDIR,       // ErrorCode 2
      EOR_INVALIDARCHIVE,   // ErrorCode 3
      EOR_INVALIDHEADERS,   // ErrorCode 4
      EOR_INVALIDHEADERVER, // ErrorCode 5
      EOR_INCOMPATIBLEVER,  // ErrorCode 6
      EOR_READERROR,        // ErrorCode 7
    };

    enum ESaveResult : int
    {
      ESR_SUCCESS = 0,
      ESR_INVALIDFILE,        // ErrorCode 1
      ESR_WRITEERROR_FOLDER,  // ErrorCode 2
      ESR_WRITEERROR_FILENAME,// ErrorCode 3
      ESR_WRITEERROR_DATA     // ErrorCode 4
    };

    BaseArchive();
    void close();

    int open(const string_t& filename);
    int open(std::istream& stream);
    int save(const string_t& filename);
    int save(std::ostream& stream);

    bool makeDir(const string_t& directoryName);
    bool removeDir(const string_t& directoryName);

    bool addFile(const string_t& filename, const string_t& archiveDirectory);
    bool addFile(std::istream& stream,
                 const string_t& filename,
                 const string_t& archiveDirectory);
    bool removeFile(const string_t& filename);
    bool extractTo(const string_t& filename, const string_t& outputDir) const;
    bool extractFile(const string_t& filename, const string_t& outputFilename) const;
    bool extractFile(const string_t& filename, std::ostream& stream) const;
    bool extractAll(const string_t& outputDirectory) const;
    bool moveFile(const string_t& filename, const string_t& newFilename);

    BaseArchiveFile* getFile(const string_t& filename);
    vector_t<string_t> getFileList() const;
    vector_t<string_t> getDirList() const;

    bool fileExists(const string_t& filename) const;
    bool dirExists(const string_t& directoryName) const;

  protected:
    using file_information_type = std::pair<string_t, std::int_least64_t>;

    EOpenResult readHeaders(std::istream& stream);

    std::pair<bool, vector_t<string_t>> readFolderList(std::istream& stream);
    std::pair<bool, vector_t<file_information_type>> readFileList(std::istream& stream);

    std::pair<std::unique_ptr<std::stringstream>, std::streamsize> readToFileStream(std::istream& stream);
    std::pair<std::unique_ptr<std::stringstream>, std::streamsize> readToFileStream(std::istream& stream, std::streamsize sizeToRead);
    bool writeFromFileStream(std::istream* fileStream, std::streamsize fileSize, std::ostream& stream) const;

    using folder_tree_type = fast_unordered_set_t<string_t>;
    using file_tree_type = fast_unordered_map_t<string_t, std::pair<std::shared_ptr<BaseArchiveFile>, std::int_least64_t>>;

    folder_tree_type  m_folderTree;
    file_tree_type    m_fileTree;
  };
}

#endif // BASEARCHIVE_H
