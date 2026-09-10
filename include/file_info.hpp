#ifndef FILE_INFO_HPP
#define FILE_INFO_HPP

#include <filesystem>
#include <fstream>
#include <map>
#include <memory>
#include <mutex>
#include <string>

class Companion;

class FileOperator
{
public:
    FileOperator(const std::filesystem::path &path);
    virtual ~FileOperator();

    virtual bool createFileAndOpen() { return false; }

    std::filesystem::path getPath() const;
    std::string getMD5Hash() const;
    bool setPath(const std::filesystem::path &path);
    std::filebuf *closeFile();

protected:
    std::filebuf buf_;
    std::filesystem::path path_;
    std::string md5Hash_;
};

class SenderOperator : public FileOperator
{
public:
    SenderOperator(const std::filesystem::path &path);
    ~SenderOperator() = default;

    bool sendFilePart(std::shared_ptr<Companion> companion, const std::string &networkId);
    void sendFile(std::shared_ptr<Companion> companion, const std::string &networkId);
};

class ReceiverOperator : public FileOperator
{
public:
    ReceiverOperator(const std::filesystem::path &path, const std::string &md5HashFromSender);
    ~ReceiverOperator() = default;

    void receiveFilePart(const std::string &value);
    bool receiveFile();

private:
    bool createFileAndOpen() override;

    std::string md5HashFromSender_;
};

class FileOperatorStorage
{
public:
    FileOperatorStorage();
    ~FileOperatorStorage() = default;

    void addSenderOperator(const std::string &networkId, const std::filesystem::path &path);

    void addReceiverOperator(
        const std::string &networkId, const std::string &md5HashFromSender,
        const std::filesystem::path &path);

    std::shared_ptr<FileOperator> getOperator(const std::string &key);

    bool removeOperator(const std::string &key);

private:
    std::mutex mutex_;
    std::map<std::string, std::shared_ptr<FileOperator>> mapping_;
};

#endif // FILE_INFO_HPP
