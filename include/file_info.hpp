#ifndef FILE_INFO_HPP
#define FILE_INFO_HPP

#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <string>

#include "logging.hpp"
#include "utils.hpp"

class Companion;

class FileOperator {
public:
    FileOperator(const std::filesystem::path&);
    virtual ~FileOperator();

    std::filesystem::path getFilePath() const;
    std::string getFileMD5Hash() const;
    bool setFilePath(const std::filesystem::path&);
    std::filebuf* closeFile();

    virtual bool createFileAndOpen() { return false; }

protected:
    std::filebuf filebuf_;
    std::filesystem::path filePath_;
    std::string fileMD5Hash_;
};

class SenderOperator : public FileOperator {
public:
    SenderOperator(const std::filesystem::path&);
    ~SenderOperator() = default;

    bool sendFilePart(Companion*, const std::string&);
    void sendFile(Companion*, const std::string&);

private:

};

class ReceiverOperator : public FileOperator {
public:
    ReceiverOperator(const std::filesystem::path&, const std::string&);
    ~ReceiverOperator() = default;

    void receiveFilePart(const std::string&);
    bool receiveFile();

private:
    std::string fileMD5HashFromSender_;

    bool createFileAndOpen() override;
};

class FileOperatorStorage {
public:
    FileOperatorStorage();
    ~FileOperatorStorage();

    void addSenderOperator(const std::string&, const std::filesystem::path&);

    void addReceiverOperator(
        const std::string&, const std::string&, const std::filesystem::path&);

    FileOperator* getOperator(const std::string&);

    bool removeOperator(const std::string&);

private:
    std::mutex mappingMutex_;
    std::map<std::string, FileOperator*> mapping_;
};

#endif // FILE_INFO_HPP
