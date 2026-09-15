#include "file_info.hpp"

#include "companion.hpp"
#include "logging.hpp"
#include "utils.hpp"

using namespace std::string_literals;

FileOperator::FileOperator(const std::filesystem::path &path) : path_(path), buf_() {}

FileOperator::~FileOperator()
{
    if (buf_.is_open())
        buf_.close();
}

std::filesystem::path FileOperator::getPath() const
{
    return path_;
}

std::string FileOperator::getMD5Hash() const
{
    return md5Hash_;
}

bool FileOperator::setPath(const std::filesystem::path &path)
{
    if (buf_.is_open())
        buf_.close();

    path_ = path;

    return createFileAndOpen();
}

std::filebuf *FileOperator::closeFile()
{
    return buf_.close();
}

SenderOperator::SenderOperator(const std::filesystem::path &path) : FileOperator(path)
{
    md5Hash_ = hashFileMD5(path_.string());

    if (!buf_.open(path_, std::ios::binary | std::ios::in))
        logTemplateError("file opening error, path: {}", path_.string());
}

bool SenderOperator::sendFilePart(
    std::shared_ptr<Companion> companion, const std::string &networkId)
{
    std::stringstream sstream;
    char buffer[MAX_BUFFER_SIZE] = { 0 };

    auto read = buf_.sgetn(buffer, MAX_BUFFER_SIZE);
    sstream << std::hex << std::setfill('0');

    for (std::size_t i = 0; i < read; i++)
        // double cast to get rid of precedence of ffffff for big values
        sstream << std::setw(2) << (int)(unsigned char)buffer[i];

    std::string resultString = sstream.str();

    bool result = companion->sendFileBlock(networkId, resultString);
    // coutWithEndl(resultString);

    return result;
}

void SenderOperator::sendFile(std::shared_ptr<Companion> companion, const std::string &networkId)
{
    auto lambda = [=, this]()
    {
        if (buf_.is_open()) {
            auto length = buf_.in_avail();
            uint32_t iterationNumber = length / MAX_BUFFER_SIZE + 1;

            for (uint32_t i = 0; i < iterationNumber; i++) {
                if (i % 100 == 0) {
                    coutArgsWithSpaceSeparator(
                        logCustomDelimiter, "iteration:", i + 1, "/", iterationNumber);
                }

                bool result = sendFilePart(companion, networkId);

                if (!result) {
                    logTemplateError(
                        "file sending stopped because of error, path: {}", path_.string());

                    // close file
                    std::filebuf *closeResult = buf_.close();

                    if (!closeResult)
                        logTemplateError("file closing error, path: {}", path_.string());

                    // remove file
                    bool removeResult = std::filesystem::remove(path_);

                    if (!removeResult)
                        logTemplateError("file {} did not exist at deletion", path_.string());

                    // send message
                    auto type = NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE;
                    companion->sendMessage(false, type, networkId, nullptr);

                    // remove self
                    auto lambda = [=]()
                    {
                        companion->removeFileOperator<SenderOperator>(networkId);
                    };

                    runInDetachedThread(lambda);

                    return;
                }
            }

            // send 'end of transmission' message
            auto typeEnd = NetworkMessageType::FILE_DATA_TRANSMISSON_END;
            companion->sendMessage(false, typeEnd, networkId, nullptr);

            if (!closeFile())
                logTemplateError("file closing error, path: {}", path_.string());
        }
        else {
            logTemplateError("file opening error, path: {}", path_.string());
        }
    };

    runInDetachedThread(lambda);
}

ReceiverOperator::ReceiverOperator(
    const std::filesystem::path &path, const std::string &md5HashFromSender) : FileOperator(path)
{
    md5Hash_ = ""s;
    md5HashFromSender_ = md5HashFromSender;

    if (path_ != path)
        createFileAndOpen();
}

void ReceiverOperator::receiveFilePart(const std::string &value)
{
    std::size_t byteSize = value.size() / 2;

    for (std::size_t i = 0; i < byteSize; i++) {
        std::string dataString(value.begin() + 2  *i, value.begin() + 2  *i + 2);
        uint8_t value = std::stoi(dataString, nullptr, 16);
        buf_.sputc(value);
    }
}

bool ReceiverOperator::receiveFile()
{
    if (!closeFile())
        logTemplateError("file closing error, path: {}", path_.string());

    md5Hash_ = hashFileMD5(path_.string());

    logArgs("fileMD5Hash_:", md5Hash_);
    logArgs("md5HashFromSender_:", md5HashFromSender_);

    return (md5Hash_ == md5HashFromSender_);
}

bool ReceiverOperator::createFileAndOpen()
{
    // create file if it does not exist
    if (!std::filesystem::exists(path_)) {
        std::ofstream stream(path_);
        stream.close();
    }

    auto openResult = buf_.open(path_, std::ios::binary | std::ios::out);

    if (!openResult)
        logTemplateError("file {} open error", path_.string());

    return (openResult) ? true : false;
}

FileOperatorStorage::FileOperatorStorage() : mutex_(), mapping_() {}

void FileOperatorStorage::addSenderOperator(
    const std::string &networkId, const std::filesystem::path &path)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (mapping_.count(networkId) != 0) {
        logTemplateError("file operator for key {} already exists", networkId);

        return;
    }

    mapping_[networkId] = std::make_shared<SenderOperator>(path);
}

void FileOperatorStorage::addReceiverOperator(
    const std::string &networkId, const std::string &md5HashFromSender,
    const std::filesystem::path &path)
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (mapping_.count(networkId) != 0) {
        logTemplateError("file operator for key {} already exists", networkId);

        return;
    }

    mapping_[networkId] = std::make_shared<ReceiverOperator>(path, md5HashFromSender);
}

std::shared_ptr<FileOperator> FileOperatorStorage::getOperator(const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex_);

    try {
        return mapping_.at(key);
    }
    catch(const std::exception &e) {
        logArgsError(e.what());
    }

    return nullptr;
}

bool FileOperatorStorage::removeOperator(const std::string &key)
{
    std::lock_guard<std::mutex> lock(mutex_);

    try {
        bool result = (mapping_.erase(key) == 1) ? true : false;

        return result;
    }
    catch(const std::exception &e) {
        logArgsError(e.what());
    }

    return false;
}
