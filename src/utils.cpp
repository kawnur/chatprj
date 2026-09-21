#include "utils.hpp"

#include <QDialog>
#include <QDialogButtonBox>
#include <QHostAddress>

#include <openssl/md5.h>
#include <openssl/evp.h>

#include "action.hpp"
#include "companion.hpp"
#include "db_interaction.hpp"
#include "file_info.hpp"
#include "logging.hpp"
#include "message.hpp"
#include "widgets_dialog.hpp"

// TODO move all constants to constants.hpp
std::string getString(const char *value)
{
    return std::string(value);
}

std::string getString(const bool &value)
{
    return (value) ? "true"s : "false"s;
}

std::string getString(std::nullptr_t value)
{
    return "nullptr_t"s;
}

std::string getString(const std::filesystem::path &value)
{
    return value.string();
}

std::string getString(const QString value)
{
    return value.toStdString();
}

bool validateCompanionName(std::vector<std::string> &errors, const std::string &name)
{
    bool result = (name.size() <= 30);

    if (!result)
        errors.emplace_back("companion name length is greater than 30");

    logArgs("validateCompanionName result:", result);

    return result;
}

bool validateIpAddress(std::vector<std::string> &errors, const std::string &ipAddress)
{
    QHostAddress address { getQString(ipAddress) };
    bool result = !(address.isNull());

    if (!result)
        errors.emplace_back("companion ipaddress is invalid");

    logArgs("validateIpAddress result:", result);

    return result;
}

bool validatePort(std::vector<std::string> &errors, const std::string &port)
{
    bool result = false;
    std::string errorMessage { "port number must be greater than 0 and lower than 65536" };

    try {
        long long portNumber = std::stoll(port, nullptr, 10);

        result = (portNumber >= 0) && (portNumber <= 65535);

        if (!result)
            errors.emplace_back(errorMessage);
    }
    catch(std::out_of_range) {
        errors.push_back(errorMessage + ", port is too big, std::out_of_range");
    }
    catch(std::invalid_argument) {
        errors.push_back(errorMessage + ", port is invalid, std::invalid_argument");
    }

    logArgs("validatePort result:", result);

    return result;
}

bool validateCompanionData(
    std::vector<std::string> &errors, std::shared_ptr<CompanionAction> action)
{
    bool nameValidationResult = validateCompanionName(errors, action->getName());
    bool ipAddressValidationResult = validateIpAddress(errors, action->getIpAddress());
    bool portValidationResult = validatePort(errors, action->getClientPort());

    bool result = nameValidationResult && ipAddressValidationResult && portValidationResult;

    logArgs("validateCompanionData result:", result);

    return result;
}

bool validatePassword(std::vector<std::string> &errors, const std::string &password)
{
    bool result = (password.size() <= 30);

    if (!result)
        errors.emplace_back("password length is greater than 30");

    logArgs("validatePassword result:", result);

    return result;
}

std::string buildDialogText(std::string &&header, const std::vector<std::string> &messages)
{
    if (messages.empty())
        return "";

    std::string text(header);

    logArgs("messages.size():", messages.size());

    for (auto &message : messages)
        text += getStringByFormat("- {}\n", message);

    return text;
}

LogType getLogTypeByDialogType(DialogType type)
{
    return getMapValue(MAP_DIALOG_TYPE_TO_LOG_TYPE, type, LogType::INFO);
}

std::string getFormattedMessageBodyString(const std::string &color, const std::string &text)
{
    return getStringByFormat("<font color=\"{0}\"><br>{1}</font>", color, text);
}

std::pair<std::string, std::string> formatMessageHeaderAndBody(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message)
{
    auto companionName = companion->getName();
    auto companionId = message->getCompanionId();
    auto authorId = message->getAuthorId();
    auto time = message->getTime();
    auto text = message->getText();

    std::string color, sender, receiver;

    if (companionId == authorId) {
        color = receivedMessageColor;
        sender = companionName;
        receiver = "Me";
    }
    else {
        color = sentMessageColor;
        sender = "Me";
        receiver = companionName;
    }

    auto header = getStringByFormat(
        "<font color=\"{0}\"><b><br><i>From {1} to {2} at {3}:</i></b></font>",
        color, sender, receiver, time);

    std::string body = getFormattedMessageBodyString(color, text);

    std::pair<std::string, std::string> data (header, body);

    return data;
}

std::string buildMessageJSONString(
    // bool isAntecedent, NetworkMessageType type, std::shared_ptr<Companion> companion,
    // const std::string &networkId, std::shared_ptr<Message> message)
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message,
    std::shared_ptr<MessageMetaData> meta)
{
    using json = nlohmann::json;

    json jsonData;

    auto networkId = message->getNetworkId();
    auto type = meta->networkMessageType_;

    jsonData["type"] = type;
    jsonData["id"] = networkId;
    jsonData["companion_id"] = companion->getId();
    jsonData["antecedent"] = message->isAntecedent();

    switch (type) {
    case NetworkMessageType::TEXT: {
        jsonData["time"] = message->getTime();
        jsonData["text"] = message->getText();
    }

    break;

    case NetworkMessageType::FILE_PROPOSAL: {
        jsonData["time"] = message->getTime();
        jsonData["text"] = message->getText();
        jsonData["hashMD5"] =
            companion->getFileOperatorStorage()->getOperator(networkId)->getMD5Hash();
    }

    break;

    case NetworkMessageType::RECEIVE_CONFIRMATION:
        jsonData["received"] = 1;

    break;

    default:
    break;
    }

    return jsonData.dump();
}

std::string buildFileBlockJSONString(
    std::shared_ptr<Companion> companion, const std::string &networkId, const std::string &data)
{
    using json = nlohmann::json;

    json jsonData;

    jsonData["type"] = NetworkMessageType::FILE_DATA;
    jsonData["id"] = networkId;
    jsonData["companion_id"] = companion->getId();
    jsonData["data"] = data;

    return jsonData.dump();
}

std::string buildChatHistoryJSONString(
    std::shared_ptr<DBReplyData> data, std::vector<std::string> &keys)
{
    using json = nlohmann::json;

    json jsonData;

    jsonData["type"] = NetworkMessageType::CHAT_HISTORY_DATA;
    jsonData["messages"] = {};

    for (std::size_t i = 0; i < data->size(); i++) {  // TODO switch to iterators
        for (auto &key : keys)
            jsonData["messages"][i][key] = data->getValue(i, key);
    }

    std::string result = jsonData.dump();

    return result;
}

nlohmann::json buildJsonObject(const std::string &jsonString)
{
    nlohmann::json jsonData = nlohmann::json::parse(jsonString);

    return jsonData;
}

std::string getRandomString(uint8_t length)
{
    std::string result(length, '_');
    std::size_t baseSize = sizeof(alphanum);

    for (int i = 0; i < length; i++)
        result.at(i) = alphanum[rand() % (baseSize - 1)];

    return result;
}

void sleepForMS(uint32_t duration)
{
    std::this_thread::sleep_for (std::chrono::milliseconds(duration));
}

bool getBoolFromDBValue(const std::string &value)
{
    const char *data = value.data();

    if (*data == 't') {
        return true;
    }
    else if (*data == 'f') {
        return false;
    }
    else {
        logTemplateError("unknown bool value from DB: {}", value);
    }

    return false;
}

std::string hashFileMD5(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
        throw std::runtime_error("Failed to open file: " + filename);

    EVP_MD_CTX *md5Context = EVP_MD_CTX_new();
    EVP_MD_CTX_init(md5Context);
    EVP_DigestInit_ex(md5Context, EVP_md5(), nullptr);

    const std::size_t bufferSize = 4096;
    char buffer[bufferSize];

    while (!file.eof()) {
        file.read(buffer, bufferSize);
        EVP_DigestUpdate(md5Context, buffer, file.gcount());
    }

    std::array<uint8_t, 16> result;
    EVP_DigestFinal_ex(md5Context, result.data(), nullptr);
    file.close();

    EVP_MD_CTX_free(md5Context);

    std::stringstream stream;

    for (auto &element : result)
        stream << std::hex << (int)element;

    return stream.str();
}
