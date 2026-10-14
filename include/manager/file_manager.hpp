#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <filesystem>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>

#include "logging.hpp"

class FileOperator;
class MessageMetaData;

class FileManager
{
public:
    FileManager();
    ~FileManager() = default;

    void addSenderOperator(const std::string &networkId, const std::filesystem::path &path);

    void addReceiverOperator(
        std::shared_ptr<MessageMetaData> meta, const std::filesystem::path &path);

    std::shared_ptr<FileOperator> getOperator(const std::string &key);

    bool removeOperator(const std::string &key);
    std::string getFilePathString(const std::string &networkId);
    bool setFileOperatorFilePath(const std::string &networkId, const std::filesystem::path &path);

    template<typename T>
    std::shared_ptr<T> getFileOperator(const std::string &networkId)
    {
        return dynamic_pointer_cast<T>(getOperator(networkId));
    }

    template<typename T>
    void removeFileOperator(const std::string &networkId)
    {
        auto fileOperator = getFileOperator<T>(networkId);

        if (fileOperator) {
            logTemplateError("file operator was not found for networkId {}", networkId);

            return;
        }

        if (!removeOperator(networkId))
            logTemplateInfo("remove file operator error for networkId {}", networkId);

        // if (fileOperator)
        //     delete fileOperator;  // TODO remove

        // logTemplateInfo("file operator for networkId {} deleted", networkId);
    }

    std::string getFileOperatorMD5Hash(const std::string &networkId);

private:
    std::mutex mutex_;
    std::unordered_map<std::string, std::shared_ptr<FileOperator>> map_;
};

#endif // FILE_MANAGER_HPP
