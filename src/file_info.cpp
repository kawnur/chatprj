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

bool FileOperator::setFilePath(const std::filesystem::path& filePath)
{
    if(this->filebuf_.is_open())
    {
        this->filebuf_.close();
    }

    this->filePath_ = filePath;

    return createFileAndOpen();
}

std::filebuf* FileOperator::closeFile()
{
    return this->filebuf_.close();
}

SenderOperator::SenderOperator(const std::filesystem::path& filePath) :
    FileOperator(filePath)
{
    fileMD5Hash_ = hashFileMD5(filePath_.string());

    if(!filebuf_.open(filePath_, std::ios::binary | std::ios::in))
    {
        logArgsErrorWithTemplate(
            "file opening error, path: %1", filePath_.string());
    }
}

bool SenderOperator::sendFilePart(Companion* companionPtr, const std::string& networkId)
{
    std::stringstream sstream;
    char buffer[maxBufferSize] = { 0 };

    auto read = this->filebuf_.sgetn(buffer, maxBufferSize);
    sstream << std::hex << std::setfill('0');

    for(size_t i = 0; i < read; i++)
    {
        // double cast to get rid of precedence of ffffff for big values
        sstream << std::setw(2) << (int)(unsigned char)buffer[i];
    }

    std::string resultString = sstream.str();

    bool result = companionPtr->sendFileBlock(networkId, resultString);
    // coutWithEndl(resultString);

    return result;
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
                if(i % 100 == 0)
                {
                    coutArgsWithSpaceSeparator(
                        logCustomDelimiter, "iteration:", i + 1, "/", iterationNumber);
                }

                bool result = this->sendFilePart(companionPtr, networkId);

                if(!result)
                {
                    logArgsErrorWithTemplate(
                        "file sending stopped because of error, path: %1",
                        this->filePath_.string());

                    // close file
                    std::filebuf* closeResult = this->filebuf_.close();

                    if(!closeResult)
                    {
                        logArgsErrorWithTemplate(
                            "file closing error, path: %1", this->filePath_.string());
                    }

                    // remove file
                    bool removeResult = std::filesystem::remove(this->filePath_);

                    if(!removeResult)
                    {
                        logArgsErrorWithTemplate(
                            "file %1 did not exist at deletion", this->filePath_.string());
                    }

                    // send message
                    companionPtr->sendMessage(
                        false, NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE,
                        networkId, nullptr);

                    // remove self
                    std::thread(
                        [=]()
                        {
                            companionPtr->removeFileOperator<SenderOperator>(networkId);
                        }).detach();

                    return;
                }
            }

            // send 'end of transmission' message
            companionPtr->sendMessage(
                false, NetworkMessageType::FILE_DATA_TRANSMISSON_END,
                networkId, nullptr);

            if(!this->closeFile())
            {
                logArgsErrorWithTemplate(
                    "file closing error, path: %1", this->filePath_.string());
            }
        }
        else
        {
            logArgsErrorWithTemplate(
                "file opening error, path: %1", this->filePath_.string());
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

    if(filePath_ != homePath)
    {
        createFileAndOpen();
    }
}

void ReceiverOperator::receiveFilePart(const std::string& filePart)
{
    size_t byteSize = filePart.size() / 2;

    for(size_t i = 0; i < byteSize; i++)
    {
        std::string dataString(filePart.begin() + 2 * i, filePart.begin() + 2 * i + 2);
        uint8_t value = std::stoi(dataString, nullptr, 16);
        this->filebuf_.sputc(value);
    }
}

bool ReceiverOperator::receiveFile()
{
    if(!this->closeFile())
    {
        logArgsErrorWithTemplate(
            "file closing error, path: %1", this->filePath_.string());
    }

    this->fileMD5Hash_ = hashFileMD5(filePath_.string());

    logArgs("this->fileMD5Hash_:", this->fileMD5Hash_);
    logArgs("this->fileMD5HashFromSender_:", this->fileMD5HashFromSender_);

    return (this->fileMD5Hash_ == this->fileMD5HashFromSender_);
}

bool ReceiverOperator::createFileAndOpen()
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
        logArgsErrorWithTemplate("file %1 open error", filePath_);
    }

    return (openResult) ? true : false;
}

FileOperatorStorage::FileOperatorStorage() :
    mappingMutex_(std::mutex()), mapping_(std::map<std::string, FileOperator*>()) {}

void FileOperatorStorage::addSenderOperator(
    const std::string& networkId, const std::filesystem::path& filePath)
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    if(this->mapping_.count(networkId) != 0)
    {
        logArgsErrorWithTemplate(
            "file operator for key %1 already exists", networkId);

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
        logArgsErrorWithTemplate(
            "file operator for key %1 already exists", networkId);

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

bool FileOperatorStorage::removeOperator(const std::string& key)
{
    std::lock_guard<std::mutex> lock(this->mappingMutex_);

    try
    {
        bool result = (this->mapping_.erase(key) == 1) ? true : false;

        return result;
    }
    catch(std::exception& e)
    {
        logArgsError(e.what());
    }

    return false;
}
