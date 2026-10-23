#include "file_manager.hpp"

#include "logging.hpp"
#include "manager.hpp"
#include "message.hpp"

FileManager::FileManager() : manager_(nullptr), mutex_(), lastOpenedPath_(HOME_PATH), map_() {}

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

    map_[networkId] = std::make_shared<SenderOperator>(shared_from_this(), path);
}

void FileManager::addReceiverOperator(MessagePtr message, const std::filesystem::path &path)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto networkId = message->networkId();

    if (map_.count(networkId) != 0) {
        logTemplateError("file operator for key {} already exists", networkId);

        return;
    }

    map_[networkId] =
        std::make_shared<ReceiverOperator>(shared_from_this(), path, message->hashMD5());
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

bool FileManager::setFilePath(const std::string &networkId, const std::filesystem::path &path)
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

void FileManager::sendFile(CompanionPtr companion, const std::string &networkId)
{
    auto sender = getOperator(networkId);

    if (!sender) {
        logTemplateError("companion has no file operator for networkId = {}", networkId);

        return;
    }

    sender->sendFile(companion, networkId);

    fileTransmissionHandler();
}

void FileManager::receiveFileData(CompanionPtr companion, MessagePtr message)
{
    logArgs(__FUNCTION__);

    auto networkId = message->networkId();
    auto receiver = getOperatorByType<ReceiverOperator>(networkId);

    if (receiver)
        receiver->receiveFilePart(message->data()->data());
    else
        logTemplateError("companion has no file operator for networkId = {}", networkId);
}

bool FileManager::mapIsEmpty()
{
    std::lock_guard<std::mutex> lock(mutex_);

    return map_.empty();
}

void FileManager::handleFileSendError(FileManagerMap::iterator iterator)
{
    auto networkId = iterator->first;
    auto _operator = iterator->second;
    auto companion = _operator->companion();

    // close file
    _operator->closeFile();

    // remove file
    _operator->removeFile();

    // remove operator
    map_.erase(iterator);

    // send message
    auto type = NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE;
    manager_->sendFileDataTransmissionResultMessage(companion, type, networkId);
}

void FileManager::handleFileSendSuccess(FileManagerMap::iterator iterator)
{
    auto networkId = iterator->first;
    auto _operator = iterator->second;
    auto companion = _operator->companion();

    // close file
    _operator->closeFile();

    // send 'end of transmission' message
    auto type = NetworkMessageType::FILE_DATA_TRANSMISSON_END;
    manager_->sendFileDataTransmissionResultMessage(companion, type, networkId);
}

void FileManager::handleOperatorState(FileManagerMap::iterator iterator)
{
    // logArgsInfo(item.second);

    auto _operator = iterator->second;

    switch (_operator->state()) {
    case FileTransmissionState::SEND_FAILURE:
        handleFileSendError(iterator);

        break;
    case FileTransmissionState::SEND_SUCCESS:
        handleFileSendSuccess(iterator);

        break;

    default:
        break;
    }
}

void FileManager::fileTransmissionHandler()
{
    auto lambda = [&]()
    {
        while (!mapIsEmpty()) {
            std::lock_guard<std::mutex> lock(mutex_);

            for (auto iter = map_.begin(); iter != map_.end(); ++iter)
                handleOperatorState(iter);

            sleepForMS(FILE_MANAGER_HANDLER_SLEEP_MS);
        }
    };

    runInDetachedThread(lambda);
}

std::optional<bool> FileManager::stopFileDataReceiving(MessagePtr message)
{
    auto networkId = message->networkId();
    auto receiver = getOperatorByType<ReceiverOperator>(networkId);

    if (!receiver)
        return std::nullopt;

    auto result = receiver->getOperationResult();

    if (result)
        removeFileOperator<ReceiverOperator>(networkId);

    return result;
}

void FileManager::stopFileDataSending(MessagePtr message)
{
    auto networkId = message->networkId();

    logTemplateInfo("companion did not receive file '{}'", getFilePathString(networkId));

    removeFileOperator<SenderOperator>(networkId);
}
