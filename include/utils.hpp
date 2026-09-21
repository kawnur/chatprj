#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <vector>

#include <nlohmann/json.hpp>

#include <QDialog>
#include <QDialogButtonBox>
#include <QString>
#include <QWidget>

#include "constants.hpp"

class ButtonInfo;
class Companion;
class CompanionAction;
class DBReplyData;
class GraphicManager;
class Message;
class MessageMetaData;

std::shared_ptr<GraphicManager> getGraphicManager();

template<typename M>
concept AssociativeContainer = requires(M m)
{
    typename M::key_type;
    typename M::mapped_type;

    // TODO add return type requirement
    // { m.at(std::declval<typename M::key_type>()) } -> std::same_as<typename M::value_type>;
    // { m.at(std::declval<typename M::key_type>()) } -> std::common_reference_with<typename M::value_type>;
    m.at(std::declval<const typename M::key_type>());
    m.at(std::declval<typename M::key_type>());
};

template <typename T>
struct isOptionalHelper : std::false_type {};

template <typename T>
struct isOptionalHelper<std::optional<T>> : std::true_type {};

template <typename T>
concept IsOptional = isOptionalHelper<std::remove_cvref_t<T>>::value;

template<typename T>
concept IsArithmetic = std::is_arithmetic_v<std::remove_cvref_t<T>>;

template<typename T>
concept IsNotArithmetic = !std::is_arithmetic_v<std::remove_cvref_t<T>>;

std::string getString(const char *value);
std::string getString(const bool &value);
std::string getString(std::nullptr_t value);
std::string getString(const std::filesystem::path &value);
std::string getString(const QString value);

template<typename T>
std::string getString(T *value)
{
    std::stringstream ss;
    ss << (void *)value;

    return ss.str();
}

template<IsOptional T>
std::string getString(T &value)
{
    return (value) ? getString(value.value()) : "EMPTY OPTIONAL";
}

template<IsArithmetic T>
std::string getString(T &&value)
{
    return std::to_string(std::forward<T>(value));
}

template<IsNotArithmetic T>
std::string getString(T &&value)
{
    if constexpr (std::is_same_v<std::remove_cvref_t<T>, QString>)
        return value.toStdString();
    if constexpr (std::is_same_v<std::remove_cvref_t<T>, std::string>)
        return value;
    else
        return std::to_string(value);
}

template<typename T>
QString getQString(T &&value)
{
    return QString::fromStdString(getString(value));
}

template<typename T, typename... Ts>
std::string getStringByFormat(T &&formatString, Ts &&...args)
{
    std::string result { "" };

    auto argsTuple = std::make_tuple(getString(std::forward<Ts>(args))...);

    auto lambda = [&](auto&... params)
    {
        auto formatArgs = std::make_format_args(params...);  // std::make_format_args needs lvalue args
        result = std::vformat(getString(formatString), formatArgs);
    };

    std::apply(lambda, argsTuple);

    return result;
}

template<typename... Ts>
QString getArgumentedQString(const QString &templateString, Ts&&... args)
{
    return templateString.arg(getQString(std::forward<Ts>(args))...);
}

template<typename... Ts>
void logArgsError(Ts &&...args);

template<typename... Ts>
void logArgsException(Ts &&...args);

template<typename T, typename... Ts>
void logTemplateError(T &&templateString, Ts &&...args);

// template<typename T, typename U>
// U getConstantMappingValue(std::string mapName, const std::map<T, U> &map, const T &key)
// {
//     try {
//         return map->at(key);
//     }
//     catch(std::out_of_range) {
//         logTemplateError("mapping {} key error", mapName);
//     }
//     catch(const std::exception &e) {
//         logArgsException(e.what());
//     }

//     return U();
// }

template<AssociativeContainer M, typename T, typename U>
// U getMappingValueOrDefault(const std::map<T, U> &map, const T &key, const U &defaultValue)
// U getMapValue(const M &map, const T &key, U &&defaultValue)
// std::optional<M::mapped_type> getMapValue(const M &map, const T &key, U &&defaultValue)
typename M::mapped_type getMapValue(const M &map, const T &key, U &&defaultValue)
{
    try {
        // return U(map.at(key));
        return map.at(key);
    }
    catch(std::out_of_range) {
        logArgsError("mapping key error");

        return defaultValue;
    }
    catch(const std::exception &e) {
        logArgsException(e.what());

        // return M::mapped_type(U());
        return (typename M::mapped_type)(std::remove_reference_t<U>());
        // return std::nullopt;
        // return U();
    }
}

template<typename F, typename... Ts>
void runInDetachedThread(F &&func, Ts&&... args)
{
    std::thread(func).detach();
}

template<typename F, typename... Ts>
void runAndCoutException(F &&func, Ts&&... args)
{
    try {
        func(args...);
    }
    catch(const std::exception &e) {
        std::cout << e.what() << std::endl;
    }
}

template<typename F, typename... Ts>
void runAndLogException(F &&func, Ts&&... args)
{
    try {
        func(args...);
    }
    catch(const std::exception &e) {
        logArgsException(e.what());
    }
}

template<typename F, typename... Ts>
bool runAndReturnBool(F &&func, Ts&&... args)
{
    try {
        func(args...);

        return true;
    }
    catch(const std::exception &e) {
        logArgsException(e.what());

        return false;
    }
}

template<typename T, typename F, typename... Ts>
std::optional<T> runAndReturnOptionalResult(F &&func, Ts&&... args)
{
    try {
        return func(args...);
    }
    catch(const std::exception &e) {
        logArgsException(e.what());

        return std::nullopt;
    }
}

// TODO get rid of T template parameter
template<typename T, typename F, typename... Ts>
std::shared_ptr<T> runAndReturnSharedPtr(F &&func, Ts&&... args)
{
    try {
        return func(args...);
    }
    catch(const std::exception &e) {
        logArgsException(e.what());

        return nullptr;
    }
}

template<typename F, typename FH, typename... Ts>
void runAndHandleException(F &&func, FH &&handler, Ts&&... args)
{
    try {
        func(args...);
    }
    catch(const std::exception &e) {
        logArgsException(e.what());
        handler(e);
    }
}

template<typename... Ts>
std::vector<std::string> buildStringVector(const Ts &...args)
{
    std::vector<std::string> result;

    (result.emplace_back(args), ...);

    return result;
}

// data validation

bool validateCompanionName(std::vector<std::string> &errors, const std::string &name);
bool validateIpAddress(std::vector<std::string> &errors, const std::string &ipAddress);
bool validatePort(std::vector<std::string> &errors, const std::string &port);

bool validateCompanionData(
    std::vector<std::string> &errors, std::shared_ptr<CompanionAction> action);

bool validatePassword(std::vector<std::string> &errors, const std::string &password);
std::string buildDialogText(std::string &&header, const std::vector<std::string> &messages);
LogType getLogTypeByDialogType(DialogType type);
std::string getFormattedMessageBodyString(const std::string &color, const std::string &text);

std::pair<std::string, std::string> formatMessageHeaderAndBody(
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message);

std::string buildMessageJSONString(
    // bool isAntecedent, NetworkMessageType type, std::shared_ptr<Companion> companion,
    // const std::string &networkId, std::shared_ptr<Message> message);
    std::shared_ptr<Companion> companion, std::shared_ptr<Message> message,
    std::shared_ptr<MessageMetaData> meta);

std::string buildFileBlockJSONString(
    std::shared_ptr<Companion> companion, const std::string &networkId, const std::string &data);

std::string buildChatHistoryJSONString(
    std::shared_ptr<DBReplyData> data, std::vector<std::string> &keys);

nlohmann::json buildJsonObject(const std::string &jsonString);
std::string getRandomString(uint8_t length);
void sleepForMS(uint32_t duration);
bool getBoolFromDBValue(const std::string &value);
std::string hashFileMD5(const std::string &filename);

#endif // UTILS_HPP
