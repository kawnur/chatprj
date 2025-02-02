#ifndef FILE_INFO_HPP
#define FILE_INFO_HPP

#include <filesystem>
#include <map>
#include <mutex>
#include <string>

#include "logging.hpp"

class FileInfo
{
public:
    FileInfo() = default;
    FileInfo(const std::filesystem::path&);
    ~FileInfo() = default;

    std::filesystem::path getFilePath();

private:
    std::filesystem::path filePath_;
};

class FileInfoStorage
{
public:
    FileInfoStorage();
    ~FileInfoStorage() = default;

    void add(const std::string&, const std::filesystem::path&);
    std::filesystem::path get(const std::string&);

private:
    std::mutex mappingMutex_;
    std::map<std::string, FileInfo> mapping_;
};

#endif // FILE_INFO_HPP
