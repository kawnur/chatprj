#include "file_info.hpp"

FileOperator::FileOperator(const std::filesystem::path& path)
    : filePath_(path), filebuf_(std::filebuf()) {}

FileOperator::~FileOperator()
{
    if (filebuf_.is_open())
        filebuf_.close();
}

std::filesystem::path FileOperator::getFilePath() const
{
    return filePath_;
}

std::string FileOperator::getFileMD5Hash() const
{
    return fileMD5Hash_;
}

bool FileOperator::setFilePath(const std::filesystem::path& filePath)
{
    if (filebuf_.is_open())
        filebuf_.close();

    filePath_ = filePath;

    return createFileAndOpen();
}

std::filebuf *FileOperator::closeFile()
{
    return filebuf_.close();
}

SenderOperator::SenderOperator(const std::filesystem::path& filePath) : FileOperator(filePath)
{
    fileMD5Hash_ = hashFileMD5(filePath_.string());

    if (!filebuf_.open(filePath_, std::ios::binary | std::ios::in))
        logTemplateError("file opening error, path: {}", filePath_.string());
}

bool SenderOperator::sendFilePart(
    std::shared_ptr<Companion> companion, const std::string& networkId)
{
    std::stringstream sstream;
    char buffer[maxBufferSize] = { 0 };

    auto read = filebuf_.sgetn(buffer, maxBufferSize);
    sstream << std::hex << std::setfill('0');

    for (std::size_t i = 0; i < read; i++)
        // double cast to get rid of precedence of ffffff for big values
        sstream << std::setw(2) << (int)(unsigned char)buffer[i];

    std::string resultString = sstream.str();

    bool result = companion->sendFileBlock(networkId, resultString);
    // coutWithEndl(resultString);

    return result;
}

void SenderOperator::sendFile(std::shared_ptr<Companion> companion, const std::string& networkId)
{
    auto sendFileLambda = [=, this]()
    {
        if (filebuf_.is_open()) {
            auto length = filebuf_.in_avail();
            uint32_t iterationNumber = length / maxBufferSize + 1;

            for (uint32_t i = 0; i < iterationNumber; i++) {
                if (i % 100 == 0) {
                    coutArgsWithSpaceSeparator(
                        logCustomDelimiter, "iteration:", i + 1, "/", iterationNumber);
                }

                bool result = sendFilePart(companion, networkId);

                if (!result) {
                    logTemplateError(
                        "file sending stopped because of error, path: {}", filePath_.string());

                    // close file
                    std::filebuf *closeResult = filebuf_.close();

                    if (!closeResult)
                        logTemplateError("file closing error, path: {}", filePath_.string());

                    // remove file
                    bool removeResult = std::filesystem::remove(filePath_);

                    if (!removeResult)
                        logTemplateError("file {} did not exist at deletion", filePath_.string());

                    // send message
                    auto type = NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE;
                    companion->sendMessage(false, type, networkId, nullptr);

                    // remove self
                    auto lambda = [=]()
                    {
                        companion->removeFileOperator<SenderOperator>(networkId);
                    };

                    std::thread(lambda).detach();

                    return;
                }
            }

            // send 'end of transmission' message
            auto typeEnd = NetworkMessageType::FILE_DATA_TRANSMISSON_END;
            companion->sendMessage(false, typeEnd, networkId, nullptr);

            if (!closeFile())
                logTemplateError("file closing error, path: {}", filePath_.string());
        }
        else {
            logTemplateError("file opening error, path: {}", filePath_.string());
        }
    };

    std::thread(sendFileLambda).detach();
}

ReceiverOperator::ReceiverOperator(
    const std::filesystem::path& filePath, const std::string& fileMD5HashFromSender)
    : FileOperator(filePath)
{
    fileMD5Hash_ = std::string("");
    fileMD5HashFromSender_ = fileMD5HashFromSender;

    if (filePath_ != homePath)
        createFileAndOpen();
}

void ReceiverOperator::receiveFilePart(const std::string& filePart)
{
    std::size_t byteSize = filePart.size() / 2;

    for (std::size_t i = 0; i < byteSize; i++) {
        std::string dataString(filePart.begin() + 2 * i, filePart.begin() + 2 * i + 2);
        uint8_t value = std::stoi(dataString, nullptr, 16);
        filebuf_.sputc(value);
    }
}

bool ReceiverOperator::receiveFile()
{
    if (!closeFile())
        logTemplateError("file closing error, path: {}", filePath_.string());

    fileMD5Hash_ = hashFileMD5(filePath_.string());

    logArgs("fileMD5Hash_:", fileMD5Hash_);
    logArgs("fileMD5HashFromSender_:", fileMD5HashFromSender_);

    return (fileMD5Hash_ == fileMD5HashFromSender_);
}

bool ReceiverOperator::createFileAndOpen()
{
    // create file if it does not exist
    if (!std::filesystem::exists(filePath_)) {
        std::ofstream stream(filePath_);
        stream.close();
    }

    auto openResult = filebuf_.open(filePath_, std::ios::binary | std::ios::out);

    if (!openResult)
        logTemplateError("file {} open error", filePath_.string());

    return (openResult) ? true : false;
}

FileOperatorStorage::FileOperatorStorage()
    : mappingMutex_(std::mutex()), mapping_(std::map<std::string, std::shared_ptr<FileOperator>>()) {}

void FileOperatorStorage::addSenderOperator(
    const std::string& networkId, const std::filesystem::path& filePath)
{
    std::lock_guard<std::mutex> lock(mappingMutex_);

    if (mapping_.count(networkId) != 0) {
        logTemplateError("file operator for key {} already exists", networkId);

        return;
    }

    mapping_[networkId] = std::make_shared<SenderOperator>(filePath);
}

void FileOperatorStorage::addReceiverOperator(
    const std::string& networkId, const std::string& fileMD5HashFromSender,
    const std::filesystem::path& filePath)
{
    std::lock_guard<std::mutex> lock(mappingMutex_);

    if (mapping_.count(networkId) != 0) {
        logTemplateError("file operator for key {} already exists", networkId);

        return;
    }

    mapping_[networkId] = std::make_shared<ReceiverOperator>(filePath, fileMD5HashFromSender);
}

std::shared_ptr<FileOperator> FileOperatorStorage::getOperator(const std::string& key)
{
    std::lock_guard<std::mutex> lock(mappingMutex_);

    try {
        return mapping_.at(key);
    }
    catch(const std::exception& e) {
        logArgsError(e.what());
    }

    return nullptr;
}

bool FileOperatorStorage::removeOperator(const std::string& key)
{
    std::lock_guard<std::mutex> lock(mappingMutex_);

    try {
        bool result = (mapping_.erase(key) == 1) ? true : false;

        return result;
    }
    catch(const std::exception& e) {
        logArgsError(e.what());
    }

    return false;
}
