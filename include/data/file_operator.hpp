#ifndef FILE_INFO_HPP
#define FILE_INFO_HPP

#include <filesystem>
#include <fstream>
#include <string>

#include "alias.hpp"
#include "constants.hpp"

class FileOperator
{
public:
    FileOperator(FileManagerPtr fileManager, const std::filesystem::path &path);
    virtual ~FileOperator();

    virtual bool createFileAndOpen() { return false; }
    virtual void sendFile(CompanionPtr companion, const std::string &networkId) {}
    virtual bool getOperationResult() { return false; }

    FileTransmissionState state();
    CompanionPtr companion() const;
    std::filesystem::path path() const;
    std::string md5Hash() const;
    void setState(FileTransmissionState state);
    void setCompanion(const CompanionPtr &companion);
    bool setPath(const std::filesystem::path &path);
    std::filebuf *closeFile();
    bool removeFile();

protected:
    FileTransmissionState state_;
    FileManagerPtr fileManager_;
    CompanionPtr companion_;
    std::filebuf buf_;
    std::filesystem::path path_;
    std::string md5Hash_;
    bool result_;
};

class SenderOperator : public FileOperator
{
public:
    SenderOperator(FileManagerPtr fileManager, const std::filesystem::path &path);
    ~SenderOperator() = default;

    bool sendFilePart(CompanionPtr companion, const std::string &networkId);
    void sendFile(CompanionPtr companion, const std::string &networkId) override;
};

class ReceiverOperator : public FileOperator
{
public:
    ReceiverOperator(
        FileManagerPtr fileManager, const std::filesystem::path &path,
        const std::string &md5HashFromSender);

    ~ReceiverOperator() = default;

    void receiveFilePart(const std::string &value);
    bool getOperationResult() override;
    void logResult();

private:
    bool createFileAndOpen() override;

    std::string md5HashFromSender_;
};

#endif // FILE_INFO_HPP
