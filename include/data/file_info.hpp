#ifndef FILE_INFO_HPP
#define FILE_INFO_HPP

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

#include "alias.hpp"

// TODO rename to file_operator.hpp

class FileOperator
{
public:
    FileOperator(const std::filesystem::path &path);
    virtual ~FileOperator();

    virtual bool createFileAndOpen() { return false; }

    std::filesystem::path path() const;
    std::string md5Hash() const;
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

    bool sendFilePart(CompanionPtr companion, const std::string &networkId);
    void sendFile(CompanionPtr companion, const std::string &networkId);
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

#endif // FILE_INFO_HPP
