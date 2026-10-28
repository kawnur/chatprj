#include "file_operator.hpp"

#include "constants.hpp"
#include "file_manager.hpp"
#include "logging.hpp"

FileOperator::FileOperator(FileManagerPtr fileManager, const std::filesystem::path &path)
    : state_(FileTransmissionState::UNKNOWN), fileManager_(fileManager), companion_(nullptr),
    path_(path), buf_(), result_(false) {}

FileOperator::~FileOperator()
{
    if (buf_.is_open())
        buf_.close();
}

FileTransmissionState FileOperator::state()
{
    return state_;
}

CompanionPtr FileOperator::companion() const
{
    return companion_;
}

std::filesystem::path FileOperator::path() const
{
    return path_;
}

std::string FileOperator::md5Hash() const
{
    return md5Hash_;
}

void FileOperator::setState(FileTransmissionState state)
{
    state_ = state;
}

void FileOperator::setCompanion(const CompanionPtr &companion)
{
    companion_ = companion;
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
    std::filebuf *result = buf_.close();

    if (!result)
        logTemplateError("file closing error, path: {}", path_);

    return result;
}

bool FileOperator::removeFile()
{
    bool result = std::filesystem::remove(path_);

    if (!result)
        logTemplateError("file removing error, path: {}", path_);

    return result;
}

SenderOperator::SenderOperator(FileManagerPtr fileManager, const std::filesystem::path &path)
    : FileOperator(fileManager, path)
{
    md5Hash_ = hashFileMD5(path_.string());

    if (!buf_.open(path_, std::ios::binary | std::ios::in))
        logTemplateError("file opening error, path: {}", path_);
}

bool SenderOperator::sendFilePart(CompanionPtr companion, const std::string &networkId)
{
    std::stringstream sstream;
    char buffer[MAX_BUFFER_SIZE] = { 0 };

    auto read = buf_.sgetn(buffer, MAX_BUFFER_SIZE);
    sstream << std::hex << std::setfill('0');

    for (std::size_t i = 0; i < read; ++i)
        // TODO get rid of double cast
        // double cast to get rid of precedence of ffffff for big values
        sstream << std::setw(2) << (int)(unsigned char)buffer[i];

    std::string resultString = sstream.str();

    bool result = fileManager_->sendFileBlock(companion, networkId, resultString);
    // coutWithEndl(resultString);

    return result;
}

void SenderOperator::sendFile(CompanionPtr companion, const std::string &networkId)
{
    setCompanion(companion);

    auto lambda = [=, this]()
    {
        if (!buf_.is_open()) {
            logTemplateError("file opening error, path: {}", path_);

            return;
        }

        auto length = buf_.in_avail();
        uint32_t iterationNumber = length / MAX_BUFFER_SIZE + 1;

        for (uint32_t i = 0; i < iterationNumber; ++i) {
            if (i % 100 == 0) {
                auto line = "{0} iteration: {1}/{2}"s;
                coutWithEndl(getStringByFormat(line, LOG_CUSTOM_DELIMITER, i + 1, iterationNumber));
            }

            bool result = sendFilePart(companion, networkId);

            if (result)
                continue;

            logTemplateError("file sending error, path: {}", path_);
            setState(FileTransmissionState::SEND_FAILURE);

            return;
        }

        setState(FileTransmissionState::SEND_SUCCESS);
    };

    runInDetachedThread(lambda);
}

ReceiverOperator::ReceiverOperator(
    FileManagerPtr fileManager, const std::filesystem::path &path,
    const std::string &md5HashFromSender)
    : FileOperator(fileManager, path)
{
    md5Hash_ = ""s;
    md5HashFromSender_ = md5HashFromSender;

    if (path_ != path)
        createFileAndOpen();
}

void ReceiverOperator::receiveFilePart(const std::string &value)
{
    std::size_t byteSize = value.size() / 2;

    for (std::size_t i = 0; i < byteSize; ++i) {
        std::string dataString(value.begin() + 2  *i, value.begin() + 2  *i + 2);
        uint8_t value = getIntFromString(DATA_BAD_VALUE, dataString, nullptr, 16);
        buf_.sputc(value);
    }
}

bool ReceiverOperator::getOperationResult()
{
    closeFile();

    md5Hash_ = hashFileMD5(path_.string());

    auto entry = "{0}, file hash, calculated: {1}, from sender: {2}"s;
    logTemplateInfo(entry, md5Hash_, md5HashFromSender_);

    result_ = (md5Hash_ == md5HashFromSender_);

    return result_;
}

void ReceiverOperator::logResult()
{
    if (result_)
        logTemplateInfo("file '{}' successfully received", path_);
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
        logTemplateError("file '{}' opening error", path_);

    return (openResult) ? true : false;
}
