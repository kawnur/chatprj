#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <filesystem>
#include <optional>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

#include "alias.hpp"
#include "file_operator.hpp"
#include "logging.hpp"

using FileManagerMap = std::unordered_map<std::string, std::shared_ptr<FileOperator>>;

class FileManager : public std::enable_shared_from_this<FileManager>
{
public:
    FileManager();
    ~FileManager() = default;

    void setManager(ManagerPtr manager);

    std::filesystem::path lastOpenedPath();
    void setLastOpenedPath(const std::filesystem::path &path);

    void addSenderOperator(const std::string &networkId, const std::filesystem::path &path);
    void addReceiverOperator(MessagePtr message, const std::filesystem::path &path = HOME_PATH);
    std::shared_ptr<FileOperator> getOperator(const std::string &key);
    bool removeOperator(const std::string &key);
    std::string getFilePathString(const std::string &networkId);
    bool setFilePath(const std::string &networkId, const std::filesystem::path &path);

    template<typename T>
    std::shared_ptr<T> getOperatorByType(const std::string &networkId)
    {
        return dynamic_pointer_cast<T>(getOperator(networkId));
    }

    template<typename T>
    void removeFileOperator(const std::string &networkId)
    {
        auto _operator = getOperatorByType<T>(networkId);

        if (!_operator) {
            logTemplateError("file operator for networkId '{}' not found ", networkId);

            return;
        }

        _operator->logResult();

        if (!removeOperator(networkId))
            logTemplateInfo("remove file operator error for networkId '{}'", networkId);

        // if (fileOperator)
        //     delete fileOperator;  // TODO remove

        // logTemplateInfo("file operator for networkId {} deleted", networkId);
    }

    std::string getFileOperatorMD5Hash(const std::string &networkId);
    bool sendFileBlock(CompanionPtr companion, const std::string &networkId, const std::string &data);
    void sendFile(CompanionPtr companion, const std::string &networkId);
    void receiveFileData(CompanionPtr companion, MessagePtr message);
    void handleFileSendError(FileManagerMap::iterator iterator);
    void handleFileSendSuccess(FileManagerMap::iterator iterator);
    void handleOperatorState(FileManagerMap::iterator iterator);
    std::optional<bool> stopFileDataReceiving(MessagePtr message);
    void stopFileDataSending(MessagePtr message);

private:
    bool mapIsEmpty();
    void fileTransmissionHandler();

    ManagerPtr manager_;
    std::mutex mutex_;
    std::filesystem::path lastOpenedPath_;
    FileManagerMap map_;
};

#endif // FILE_MANAGER_HPP
