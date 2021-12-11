#ifndef COMMANDS_H
#define COMMANDS_H

#include "../../Game/src/core/archive_type.h"
#include "../../Game/src/core/type.h"

void perfChangeDir(const gengine2d::archive_t& archive,
                   const gengine2d::string_t& dir,
                   gengine2d::string_t& currentDir);
void perfMakeDir(gengine2d::archive_t& archive,
                 const gengine2d::string_t& dir,
                 const gengine2d::string_t& currentDir);

void perfListDir(const gengine2d::vector_t<gengine2d::string_t>& folderList,
                 const gengine2d::string_t& currentDir,
                 bool colored = true);
void perfListDirCount(const gengine2d::vector_t<gengine2d::string_t>& folderList,
                      const gengine2d::string_t& currentDir);
void perfListFile(const gengine2d::vector_t<gengine2d::string_t>& fileList,
                  const gengine2d::string_t& currentDir);
void perfListFileCount(const gengine2d::vector_t<gengine2d::string_t>& fileList,
                       const gengine2d::string_t& currentDir);
void perfListFileDetail(const gengine2d::archive_t& archive,
                        const gengine2d::vector_t<gengine2d::string_t>& fileList,
                        const gengine2d::string_t& currentDir);

void perfAdd(gengine2d::archive_t& archive,
             const gengine2d::string_t& filename,
             const gengine2d::string_t& currentDir);
void perfRemove(gengine2d::archive_t& archive,
                const gengine2d::string_t& filename,
                const gengine2d::string_t& currentDir);
void perfExtract(gengine2d::archive_t& archive,
                 const gengine2d::string_t& filename,
                 const gengine2d::string_t& currentDir,
                 gengine2d::string_t outputDir);
void perfExtractAll(gengine2d::archive_t& archive,
                    gengine2d::string_t outputDir);
void perfSave(gengine2d::archive_t& archive,
              const gengine2d::string_t& filename);

void printHelp(const gengine2d::string_t program);
void printInvalidUsage(const gengine2d::string_t& cmd);
void printSuccess();

#endif // COMMANDS_H
