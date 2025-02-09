#include "file_info.hpp"

FileOperator::FileOperator(const std::filesystem::path& path) :
    filePath_(path), filebuf_(std::filebuf()) {}

FileOperator::~FileOperator()
{
    if(this->filebuf_.is_open())
    {
        this->filebuf_.close();
    }
}

std::filesystem::path FileOperator::getFilePath() const
{
    return this->filePath_;
}

std::string FileOperator::getFileMD5Hash() const
{
    return this->fileMD5Hash_;
}

void FileOperator::setFilePath(const std::filesystem::path& filePath)
{
    if(this->filebuf_.is_open())
    {
        this->filebuf_.close();
    }

    this->filePath_ = filePath;

    createFileAndOpen();
}

void FileOperator::closeFile()
{
    auto isOpen = this->filebuf_.is_open();
    auto result = this->filebuf_.close();
}

SenderOperator::SenderOperator(const std::filesystem::path& filePath) :
    FileOperator(filePath)
{
    fileMD5Hash_ = hashFileMD5(filePath_.string());
    filebuf_.open(filePath_, std::ios::binary | std::ios::in);
}

void SenderOperator::sendFilePart(Companion* companionPtr, const std::string& networkId)
{
    char buffer[maxBufferSize] = { 0 };

    auto read = this->filebuf_.sgetn(buffer, maxBufferSize);

    std::string bufferString(buffer);

    if(*(bufferString.end() - 1) == 0x02)
    {
        bufferString.erase(bufferString.end() - 1);
    }

    bool result = companionPtr->sendFileBlock(networkId, bufferString);    
}

void SenderOperator::sendFile(Companion* companionPtr, const std::string& networkId)
{
    auto sendFileLambda = [=, this]()
    {
        if(this->filebuf_.is_open())
        {
            auto length = this->filebuf_.in_avail();

            uint32_t iterationNumber = length / maxBufferSize + 1;

            for(uint32_t i = 0; i < iterationNumber; i++)
            {
                this->sendFilePart(companionPtr, networkId);
            }

            // send 'end of transmission' message
            companionPtr->sendMessage(
                false, NetworkMessageType::FILE_DATA_TRANSMISSON_END,
                networkId, nullptr);

            this->closeFile();
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

ReceiverOperator::ReceiverOperator(
    const std::filesystem::path& filePath, const std::string& fileMD5HashFromSender) :
    FileOperator(filePath)
{
    fileMD5Hash_ = std::string("");
    fileMD5HashFromSender_ = fileMD5HashFromSender;

    createFileAndOpen();
}

void ReceiverOperator::receiveFilePart(const std::string& filePart)
{
    auto isOpen = this->filebuf_.is_open();
    this->filebuf_.sputn(filePart.data(), filePart.size());
}

bool ReceiverOperator::receiveFile()
{
    this->closeFile();

    this->fileMD5Hash_ = hashFileMD5(filePath_.string());

    logArgs("this->fileMD5Hash_:", this->fileMD5Hash_);
    logArgs("this->fileMD5HashFromSender_:", this->fileMD5HashFromSender_);

    return (this->fileMD5Hash_ == this->fileMD5HashFromSender_);
}

void ReceiverOperator::createFileAndOpen()
{
    // create file if it does not exist
    if(!std::filesystem::exists(filePath_))
    {
        std::ofstream stream(filePath_);
        stream.close();
    }

    auto openResult = filebuf_.open(filePath_, std::ios::binary | std::ios::out);

    if(!openResult)
    {
        logArgsError(
            QString("file %1 open error"),
            getQString(filePath_.string()));
    }
}

FileOperatorStorage::FileOperatorStorage() :
    mappingMutex_(std::mutex()), mapping_(std::map<std::string, FileOperator*>()) {}

void FileOperatorStorage::addSenderOperator(
    const std::string& networkId, const std::filesystem::path& filePath)
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    if(this->mapping_.count(networkId) != 0)
    {
        logArgsError(
            QString("file operator for key %1 already exists")
                .arg(getQString(networkId)));

        return;
    }

    this->mapping_[networkId] = new SenderOperator(filePath);
}

void FileOperatorStorage::addReceiverOperator(
    const std::string& networkId, const std::string& fileMD5HashFromSender,
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

    this->mapping_[networkId] = new ReceiverOperator(filePath, fileMD5HashFromSender);
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
