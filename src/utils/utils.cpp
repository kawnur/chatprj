#include "utils.hpp"

#include <fstream>

#include <QDialog>
#include <QDialogButtonBox>
// #include <QHostAddress>

#include <openssl/md5.h>
#include <openssl/evp.h>

#include "constants.hpp"
#include "logging.hpp"

// TODO move all constants to constants.hpp
std::string getString(const char *value)
{
    return std::string(value);
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

uint32_t getWidgetColor(Widget value)
{
    return getMapValue(WIDGET_COLOR_MAP, value, DEFAULT_WIDGET_COLOR);
}

std::string getHTMLColor(Widget value)
{
    return std::format("#{:06x}", getWidgetColor(value));
}

std::string buildTextAsUnorderedList(const std::vector<std::string> &messages)
{
    if (messages.empty())
        return "";

    auto text = ""s;

    logArgs("messages.size():", messages.size());

    for (auto &message : messages)
        text += getStringByFormat("- {}\n", message);

    return text;
}

std::string buildTextAsUnorderedListWithHeader(
    const std::string &header, const std::vector<std::string> &messages)
{
    if (messages.empty())
        return "";

    auto text = getStringByFormat("{}:\n\n", header);
    text += buildTextAsUnorderedList(messages);

    return text;
}

LogType getLogTypeByDialogType(DialogType type)
{
    return getMapValue(MAP_DIALOG_TYPE_TO_LOG_TYPE, type, LogType::INFO);
}

std::string getHTMLHeader(
    const std::string &color, const std::string &sender, const std::string &receiver,
    const std::string &time)
{
    auto headerTemplate = "<font color=\"{0}\"><b><br><i>From {1} to {2} at {3}:</i></b></font>"s;

    return getStringByFormat(headerTemplate, color, sender, receiver, time);
}

std::string getHTMLBody(const std::string &color, const std::string &text, bool _break, bool bold)
{
    auto breakSub = (_break) ? "<br>"s : ""s;
    auto boldOpen = (bold) ? "<b>"s : ""s;
    auto boldClose = (bold) ? "</b>"s : ""s;

    auto textSub = getStringByFormat("{0}{1}{2}{3}", breakSub, boldOpen, text, boldClose);

    return getStringByFormat("<font color=\"{0}\">{1}</font>", color, textSub);
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

void exitUtil(int result)
{
    logArgsInfo(EXIT_LOG_ENTRY);

    std::exit(result);
}
