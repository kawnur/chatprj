#include "file_info.hpp"

FileInfo::FileInfo(const std::filesystem::path& path) : filePath_(path) {}

std::filesystem::path FileInfo::getFilePath()
{
    return this->filePath_;
}

FileInfoStorage::FileInfoStorage() : mapping_(std::map<std::string, FileInfo>()) {}

void FileInfoStorage::add(
    const std::string& networkId, const std::filesystem::path& filePath)
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    this->mapping_[networkId] = filePath;
}

std::filesystem::path FileInfoStorage::get(const std::string& key)
{
    try
    {
        return this->mapping_.at(key).getFilePath();
    }
    catch(std::exception& e)
    {
        logArgsError(e.what());
    }

    return std::filesystem::path("");
}
