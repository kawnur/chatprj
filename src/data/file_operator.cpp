#include "file_operator.hpp"

#include "constants.hpp"
#include "file_manager.hpp"
#include "logging.hpp"
// #include "manager.hpp"
#include "message.hpp"

FileOperator::FileOperator(FileManagerPtr fileManager, const std::filesystem::path &path)
    : fileManager_(fileManager), path_(path), buf_() {}

FileOperator::~FileOperator()
{
    if (buf_.is_open())
        buf_.close();
}

std::filesystem::path FileOperator::path() const
{
    return path_;
}

std::string FileOperator::md5Hash() const
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

SenderOperator::SenderOperator(FileManagerPtr fileManager, const std::filesystem::path &path)
    : FileOperator(fileManager, path)
{
    md5Hash_ = hashFileMD5(path_.string());

    if (!buf_.open(path_, std::ios::binary | std::ios::in))
        logTemplateError("file opening error, path: {}", path_.string());
}

bool SenderOperator::sendFilePart(CompanionPtr companion, const std::string &networkId)
{
    std::stringstream sstream;
    char buffer[MAX_BUFFER_SIZE] = { 0 };

    auto read = buf_.sgetn(buffer, MAX_BUFFER_SIZE);
    sstream << std::hex << std::setfill('0');

    for (std::size_t i = 0; i < read; i++)
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
    auto getReplyMeta = [&](NetworkMessageType type)
    {
        auto meta = std::make_shared<MessageMetaData>();
        meta->networkMessageType_ = type;
        meta->networkId_ = networkId;

        return meta;
    };

    auto getEmptyMessage = []()
    {
        auto state = std::make_shared<MessageState>();
        state->isAntecedent_ = false;

        auto message = std::make_shared<Message>(nullptr, nullptr, state);

        return message;
    };

    auto lambda = [=, this]()
    {
        if (!buf_.is_open()) {
            logTemplateError("file opening error, path: {}", path_.string());

            return;
        }

        auto length = buf_.in_avail();
        uint32_t iterationNumber = length / MAX_BUFFER_SIZE + 1;

        for (uint32_t i = 0; i < iterationNumber; i++) {
            if (i % 100 == 0) {
                std::string line { "{0} iteration: {1}/{2}" };
                coutWithEndl(getStringByFormat(line, logCustomDelimiter, i + 1, iterationNumber));
            }

            bool result = sendFilePart(companion, networkId);

            if (result)
                continue;

            // hanle error
            logTemplateError("file sending stopped because of error, path: {}", path_.string());

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
            getManager()->sendMessage(companion, getEmptyMessage(), getReplyMeta(type));

            // remove self
            auto lambda = [=]()
            {
                getManager()->removeFileOperator<SenderOperator>(networkId);
            };

            runInDetachedThread(lambda);

            return;
        }

        // send 'end of transmission' message
        auto typeEnd = NetworkMessageType::FILE_DATA_TRANSMISSON_END;
        getManager()->sendMessage(companion, getEmptyMessage(), getReplyMeta(typeEnd));

        if (!closeFile())
            logTemplateError("file closing error, path: {}", path_.string());
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

    for (std::size_t i = 0; i < byteSize; i++) {
        std::string dataString(value.begin() + 2  *i, value.begin() + 2  *i + 2);
        uint8_t value = getIntFromString(DATA_BAD_VALUE, dataString, nullptr, 16);
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
