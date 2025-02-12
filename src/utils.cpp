#include "utils.hpp"

// TODO move all constants to constants.hpp

bool validateCompanionName(
    std::vector<std::string>& validationErrors, const std::string& name)
{
    bool result = (name.size() <= 30);

    if(!result)
    {
        validationErrors.push_back(std::string("companion name length is greater than 30"));
    }

    logArgs("validateCompanionName result:", result);

    return result;
}

bool validateIpAddress(
    std::vector<std::string>& validationErrors, const std::string& ipAddress)
{
    QHostAddress address { getQString(ipAddress) };

    bool result = !(address.isNull());

    if(!result)
    {
        validationErrors.push_back(std::string("companion ipaddress is invalid"));
    }

    logArgs("validateIpAddress result:", result);

    return result;
}

bool validatePort(std::vector<std::string>& validationErrors, const std::string& port)
{
    bool result = false;

    std::string errorMessage("port number must be greater than 0 and lower than 65536");

    try
    {
        long long portNumber = std::stoll(port, nullptr, 10);

        result = (portNumber >= 0) && (portNumber <= 65535);

        if(!result)
        {
            validationErrors.push_back(errorMessage);
        }
    }
    catch(std::out_of_range)
    {
        validationErrors.push_back(
            errorMessage + std::string(", port number is too big, std::out_of_range"));
    }
    catch(std::invalid_argument)
    {
        validationErrors.push_back(
            errorMessage + std::string(", port number is invalid, std::invalid_argument"));
    }

    logArgs("validatePort result:", result);

    return result;
}

bool validateCompanionData(
    std::vector<std::string>& errors,
    const CompanionAction* actionPtr)
{
    bool nameValidationResult = validateCompanionName(errors, actionPtr->getName());
    bool ipAddressValidationResult = validateIpAddress(errors, actionPtr->getIpAddress());
    bool portValidationResult = validatePort(errors, actionPtr->getClientPort());

    bool result =
        nameValidationResult && ipAddressValidationResult && portValidationResult;

    logArgs("validateCompanionData result:", result);

    return result;
}

bool validatePassword(
    std::vector<std::string>& validationErrors, const std::string& password)
{
    bool result = (password.size() <= 30);

    if(!result)
    {
        validationErrors.push_back(std::string("password length is greater than 30"));
    }

    logArgs("validatePassword result:", result);

    return result;
}

QString buildDialogText(std::string&& header, const std::vector<std::string>& messages)
{
    if(messages.empty())
    {
        return "";
    }
    else
    {
        std::string text(header);

        logArgs("messages.size():", messages.size());

        for(auto& message : messages)
        {
            text += (std::string("- ") + message + std::string("\n"));
        }

        return getQString(text);
    }
}

std::vector<ButtonInfo>* createOkButtonInfoVector(void (TextDialog::*functionPtr)())
{
    std::vector<ButtonInfo>* vectorPtr = new std::vector<ButtonInfo>;

    vectorPtr->emplace_back(
        okButtonText, QDialogButtonBox::AcceptRole, functionPtr);

    return vectorPtr;
}

void showInfoDialogAndLogInfo(
    QWidget* parentPtr, const QString& message, void (TextDialog::*functionPtr)())
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::INFO, message,
        createOkButtonInfoVector(functionPtr));

    logArgsInfo(message);
}

void showInfoDialogAndLogInfo(QWidget* parentPtr, QString&& message)
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::INFO, std::move(message),
        createOkButtonInfoVector(&QDialog::accept));

    logArgsInfo(message);
}

void showWarningDialogAndLogWarning(QWidget* parentPtr, const QString& message)
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::WARNING, message,
        createOkButtonInfoVector(&QDialog::accept));

    logArgsWarning(message);
}

void showErrorDialogAndLogError(QWidget* parentPtr, const QString& message)
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::ERROR, message,
        createOkButtonInfoVector(&QDialog::accept));

    logArgsError(message);
}

void showErrorDialogAndLogError(QWidget* parentPtr, QString&& message)
{
    getGraphicManagerPtr()->createTextDialogAndShow(
        parentPtr, DialogType::ERROR, std::move(message),
        createOkButtonInfoVector(&QDialog::accept));

    logArgsError(message);
}

QString getFormattedMessageBodyQString(const QString& color, const QString& text)
{
    return getArgumentedQString("<font color=\"%1\"><br>%2</font>", color, text);
}

std::pair<QString, QString> formatMessageHeaderAndBody(
    const Companion* companionPtr, const Message* messagePtr)
{
    auto companionNameQString = getQString(companionPtr->getName());
    auto companionId = messagePtr->getCompanionId();
    auto authorId = messagePtr->getAuthorId();
    auto time = getQString(messagePtr->getTime());
    auto text = getQString(messagePtr->getText());

    QString color, sender, receiver;

    if(companionId == authorId)
    {
        color = receivedMessageColor;
        sender = companionNameQString;
        receiver = "Me";
    }
    else
    {
        color = sentMessageColor;
        sender = "Me";
        receiver = companionNameQString;
    }

    QString header = getArgumentedQString(
        "<font color=\"%1\"><b><br><i>From %2 to %3 at %4:</i></b></font>",
        color, sender, receiver, time);

    QString body = getFormattedMessageBodyQString(color, text);

    std::pair<QString, QString> data (header, body);

    return data;
}

std::string buildMessageJSONString(
    bool isAntecedent, NetworkMessageType type, const Companion* companionPtr,
    const std::string& networkId, const Message* messagePtr)
{
    using json = nlohmann::json;

    json jsonData;

    jsonData["type"] = type;
    jsonData["id"] = networkId;
    jsonData["companion_id"] = companionPtr->getId();
    jsonData["antecedent"] = isAntecedent;

    switch(type)
    {
    case NetworkMessageType::TEXT:
        jsonData["time"] = messagePtr->getTime();
        jsonData["text"] = messagePtr->getText();

        break;

    case NetworkMessageType::FILE_PROPOSAL:
        jsonData["time"] = messagePtr->getTime();
        jsonData["text"] = messagePtr->getText();
        jsonData["hashMD5"] =
            companionPtr->getFileOperatorStoragePtr()->
                getOperator(networkId)->getFileMD5Hash();

        break;

    case NetworkMessageType::RECEIVE_CONFIRMATION:
        jsonData["received"] = 1;

        break;

    case NetworkMessageType::RECEIVE_CONFIRMATION_REQUEST:
    case NetworkMessageType::CHAT_HISTORY_REQUEST:
    case NetworkMessageType::FILE_REQUEST:
    case NetworkMessageType::FILE_DATA_TRANSMISSON_END:
    case NetworkMessageType::FILE_DATA_CHECK_SUCCESS:
    case NetworkMessageType::FILE_DATA_CHECK_FAILURE:
    case NetworkMessageType::FILE_DATA_TRANSMISSON_FAILURE:

        break;
    }

    return jsonData.dump();
}

std::string buildFileBlockJSONString(
    const Companion* companionPtr, const std::string& networkId, const std::string& data)
{
    using json = nlohmann::json;

    json jsonData;

    jsonData["type"] = NetworkMessageType::FILE_DATA;
    jsonData["id"] = networkId;
    jsonData["companion_id"] = companionPtr->getId();
    jsonData["data"] = data;

    return jsonData.dump();
}

std::string buildChatHistoryJSONString(
    std::shared_ptr<DBReplyData>& dataPtr, std::vector<std::string>& keys)
{
    using json = nlohmann::json;
    json jsonData;

    jsonData["type"] = NetworkMessageType::CHAT_HISTORY_DATA;
    jsonData["messages"] = {};

    for(size_t i = 0; i < dataPtr->size(); i++)  // TODO switch to iterators
    {
        for(auto& key : keys)
        {
            jsonData["messages"][i][key] = dataPtr->getValue(i, key);
        }
    }

    std::string result = jsonData.dump();

    return result;
}

nlohmann::json buildJsonObject(const std::string& jsonString)
{
    nlohmann::json jsonData = nlohmann::json::parse(jsonString);

    return jsonData;
}

std::string getRandomString(uint8_t length)
{
    std::string result(length, '_');
    size_t baseSize = sizeof(alphanum);

    for(int i = 0; i < length; i++)
    {
        result.at(i) = alphanum[rand() % (baseSize - 1)];
    }

    return result;
}

void sleepForMilliseconds(uint32_t duration)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

bool getBoolFromDBValue(const char* valuePtr)
{
    if(*valuePtr == 't')
    {
        return true;
    }
    else if(*valuePtr == 'f')
    {
        return false;
    }
    else
    {
        logArgsErrorWithTemplate("unknown bool value from DB: %1", valuePtr);
    }

    return false;
}

std::string hashFileMD5(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    EVP_MD_CTX* md5Context = EVP_MD_CTX_new();
    EVP_MD_CTX_init(md5Context);
    EVP_DigestInit_ex(md5Context, EVP_md5(), nullptr);

    const size_t bufferSize = 4096;
    char buffer[bufferSize];

    while (!file.eof())
    {
        file.read(buffer, bufferSize);
        EVP_DigestUpdate(md5Context, buffer, file.gcount());
    }

    std::array<uint8_t, 16> result;
    EVP_DigestFinal_ex(md5Context, result.data(), nullptr);
    file.close();

    EVP_MD_CTX_free(md5Context);

    std::stringstream stream;

    for(auto& element : result)
    {
        stream << std::hex << (int)element;
    }

    return stream.str();
}
