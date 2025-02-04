#ifndef FILE_INFO_HPP
#define FILE_INFO_HPP

#include <filesystem>
#include <map>
#include <mutex>
#include <string>

#include "logging.hpp"

class FileOperator
{
public:
    FileOperator(bool, const std::filesystem::path&);
    ~FileOperator() = default;

    bool getIsSender() const;
    std::filesystem::path getFilePath() const;
    void setFilePath(const std::filesystem::path&);

    void send();
    void sendFile();
    void receive();
    void receiveFile();

private:
    bool isSender_;
    std::filesystem::path filePath_;
};

class FileOperatorStorage
{
public:
    FileOperatorStorage();
    ~FileOperatorStorage();

    void addOperator(const std::string&, bool);
    void addOperator(const std::string&, bool, const std::filesystem::path&);
    FileOperator* getOperator(const std::string&);

private:
    std::mutex mappingMutex_;
    std::map<std::string, FileOperator*> mapping_;
};

#endif // FILE_INFO_HPP
