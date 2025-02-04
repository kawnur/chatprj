#include "file_info.hpp"

FileOperator::FileOperator(bool isSender, const std::filesystem::path& path) :
    isSender_(isSender), filePath_(path) {}

bool FileOperator::getIsSender()
{
    return this->isSender_;
}

std::filesystem::path FileOperator::getFilePath()
{
    return this->filePath_;
}

void FileOperator::setFilePath(const std::filesystem::path& filePath)
{
    this->filePath_ = filePath;
}

void FileOperator::send()
{

}

void FileOperator::sendFile()
{

}

void FileOperator::receive()
{

}

void FileOperator::receiveFile()
{

}

FileOperatorStorage::FileOperatorStorage() :
    mappingMutex_(std::mutex()), mapping_(std::map<std::string, FileOperator*>()) {}

void FileOperatorStorage::addOperator(
    const std::string& networkId, bool isSender)
{
    this->addOperator(networkId, isSender, std::filesystem::path("~"));
}

void FileOperatorStorage::addOperator(
    const std::string& networkId, bool isSender,
    const std::filesystem::path& filePath)
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    if(this->mapping_.count(networkId) != 0)
    {
        logArgsError(
            QString("file operator for key %1 already exists")
                .arg(getQString(networkId)));

        return;
    }

    this->mapping_[networkId] = new FileOperator(isSender, filePath);
}

FileOperatorStorage::~FileOperatorStorage()
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    for(auto& pair : this->mapping_)
    {
        delete pair.second;
    }
}

FileOperator* FileOperatorStorage::getOperator(const std::string& key)
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    try
    {
        return this->mapping_.at(key);
    }
    catch(std::exception& e)
    {
        logArgsError(e.what());
    }

    return nullptr;
}
