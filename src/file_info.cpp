#include "file_info.hpp"

FileOperator::FileOperator(bool isSender, const std::filesystem::path& path) :
    isSender_(isSender), filePath_(path) {}

bool FileOperator::getIsSender()
{
    return this->isSender_;
}

std::filesystem::path FileOperator::getFilePath()
{
    return this->filePath_;
}

void FileOperator::setFilePath(const std::filesystem::path& filePath)
{
    this->filePath_ = filePath;
}

void FileOperator::send()
{

}

void FileOperator::sendFile(Companion* companionPtr, const std::string& networkId)
{
    auto sendFileLambda = [this, =]()
    {
        std::filebuf filebuf;

        filebuf.open(this->filePath_, std::ios::binary | std::ios::in);

        if(filebuf.is_open())
        {
            auto length = filebuf.in_avail();

            if(length < maxBufferSize)  // TODO loop instead of if-clause
            {
                char buffer[length] = { 0 };

                auto read = readFileBuf.sgetn(buffer, length);

                std::string bufferString(buffer);

                filebuf.close();

                bool result = companionPtr->sendFileBlock(networkId, bufferString);
            }
            else
            {
                // read file block and send block iteratively

            }
        }
        else
        {
            logArgsError(
                QString("file opening error, path: %1")
                    .arg(getQString(this->filePath_.string())));
        }
    };

    std::thread(sendFileLambda).detach();
}

void FileOperator::receive()
{

}

void FileOperator::receiveFile()
{

}

FileOperatorStorage::FileOperatorStorage() :
    mappingMutex_(std::mutex()), mapping_(std::map<std::string, FileOperator*>()) {}

void FileOperatorStorage::addOperator(
    const std::string& networkId, bool isSender)
{
    this->addOperator(networkId, isSender, std::filesystem::path("~"));
}

void FileOperatorStorage::addOperator(
    const std::string& networkId, bool isSender,
    const std::filesystem::path& filePath)
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    if(this->mapping_.count(networkId) != 0)
    {
        logArgsError(
            QString("file operator for key %1 already exists")
                .arg(getQString(networkId)));

        return;
    }

    this->mapping_[networkId] = new FileOperator(isSender, filePath);
}

FileOperatorStorage::~FileOperatorStorage()
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    for(auto& pair : this->mapping_)
    {
        delete pair.second;
    }
}

FileOperator* FileOperatorStorage::getOperator(const std::string& key)
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    try
    {
        return this->mapping_.at(key);
    }
    catch(std::exception& e)
    {
        logArgsError(e.what());
    }

    return nullptr;
}
