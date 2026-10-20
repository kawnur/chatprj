#ifndef FILE_MANAGER_HPP
#define FILE_MANAGER_HPP

#include <filesystem>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <string>

#include "alias.hpp"
#include "logging.hpp"

class FileManager : public std::enable_shared_from_this<FileManager>
{
public:
    FileManager();
    ~FileManager() = default;

    void setManager(ManagerPtr manager);

    std::filesystem::path lastOpenedPath();
    void setLastOpenedPath(const std::filesystem::path &path);

    void addSenderOperator(const std::string &networkId, const std::filesystem::path &path);
    void addReceiverOperator(MessageMetaDataPtr meta, const std::filesystem::path &path = HOME_PATH);
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
    bool sendFileBlock(CompanionPtr companion, const std::string &networkId, const std::string &data);

private:
    ManagerPtr manager_;
    std::mutex mutex_;
    std::filesystem::path lastOpenedPath_;
    std::unordered_map<std::string, std::shared_ptr<FileOperator>> map_;
};

#endif // FILE_MANAGER_HPP
