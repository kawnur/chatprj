#include "file_info.hpp"

FileOperator::FileOperator(bool isSender, const std::filesystem::path& path) :
    isSender_(isSender), filePath_(path), filebuf_(std::filebuf())
{
    std::ios_base::openmode mode = (isSender_) ? std::ios::in : std::ios::out;

    filebuf_.open(path_, std::ios::binary | mode);
}

FileOperator::~FileOperator()
{
    if(this->filebuf_.is_open())
    {
        this->filebuf_.close();
    }
}

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

void FileOperator::sendFilePart(Companion* companionPtr, const std::string& networkId)
{
    char buffer[maxBufferSize] = { 0 };

    auto read = this->filebuf_.sgetn(buffer, maxBufferSize);

    std::string bufferString(buffer);

    bool result = companionPtr->sendFileBlock(networkId, bufferString);

    delete[] buffer;
}

void FileOperator::sendFile(Companion* companionPtr, const std::string& networkId)
{
    auto sendFileLambda = [this, =]()
    {
        if(this->filebuf_.is_open())
        {
            auto length = filebuf.in_avail();

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

void FileOperator::receiveFilePart(const std::string& filePart)
{
    this->filebuf_.sputn(filePart.data(), filePart.size());
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
