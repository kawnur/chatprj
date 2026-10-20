#include "file_manager.hpp"

#include "file_operator.hpp"
#include "logging.hpp"
#include "manager.hpp"
#include "message.hpp"

FileManager::FileManager() : mutex_(), lastOpenedPath_(HOME_PATH), map_() {}

void FileManager::setManager(ManagerPtr manager)
{
    manager_ = manager;
}

std::filesystem::path FileManager::lastOpenedPath()
{
    return lastOpenedPath_;
}

void FileManager::setLastOpenedPath(const std::filesystem::path &path)
{
    lastOpenedPath_ = path;
}

void FileManager::addSenderOperator(const std::string &networkId, const std::filesystem::path &path)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (map_.count(networkId) != 0) {
        logTemplateError("file operator for key {} already exists", networkId);

        return;
    }

    map_[networkId] = std::make_shared<SenderOperator>(path);
}

void FileManager::addReceiverOperator(MessageMetaDataPtr meta, const std::filesystem::path &path)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto networkId = meta->networkId_;

    if (map_.count(networkId) != 0) {
        logTemplateError("file operator for key {} already exists", networkId);

        return;
    }

    map_[networkId] = std::make_shared<ReceiverOperator>(path, meta->hashMD5_);
}

std::shared_ptr<FileOperator> FileManager::getOperator(const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex_);

    try {
        return map_.at(key);
    }
    catch(const std::exception &e) {
        logArgsError(e.what());
    }

    return nullptr;
}

bool FileManager::removeOperator(const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex_);

    try {
        bool result = (map_.erase(key) == 1) ? true : false;

        return result;
    }
    catch(const std::exception &e) {
        logArgsError(e.what());
    }

    return false;
}

std::string FileManager::getFilePathString(const std::string &networkId)
{
    auto _operator = getOperator(networkId);

    return (_operator) ? _operator->path().string() : ""s;
}

bool FileManager::setFileOperatorFilePath(
    const std::string &networkId, const std::filesystem::path &path)
{
    auto _operator = getOperator(networkId);

    return (_operator) ? _operator->setPath(path) : false;
}

std::string FileManager::getFileOperatorMD5Hash(const std::string &networkId)
{
    auto _operator = getOperator(networkId);

    return (_operator) ? _operator->md5Hash() : ""s;
}

bool FileManager::sendFileBlock(
    CompanionPtr companion, const std::string &networkId, const std::string &data)
{
    return manager_->sendFileBlock(companion, networkId, data);
}

