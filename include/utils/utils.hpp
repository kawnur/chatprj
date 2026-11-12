#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <format>
#include <functional>
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
#include <QHostAddress>
#include <QString>
#include <QWidget>

#include "alias.hpp"
#include "constants.hpp"

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

template<typename T>
concept IsArithmetic = std::is_arithmetic_v<std::remove_cvref_t<T>>;

template<typename T>
concept IsNotArithmetic = !std::is_arithmetic_v<std::remove_cvref_t<T>>;

template <typename T>
struct isOptionalHelper : std::false_type {};

template <typename T>
struct isOptionalHelper<std::optional<T>> : std::true_type {};

template <typename T>
concept IsOptional = isOptionalHelper<std::remove_cvref_t<T>>::value;

std::string getString(const char *value);
std::string getString(std::nullptr_t value);

template<typename T>
std::string getString(T *value)
{
    std::stringstream ss;
    ss << (void *)value;

    return ss.str();
}

template<typename T, typename U>
concept Same = std::is_same_v<std::remove_cvref_t<T>, U>;

template<IsArithmetic T>
std::string getString(T &&value)
{
    if constexpr (Same<T, bool>)
        return (value) ? "true"s : "false"s;
    else
        return std::to_string(std::forward<T>(value));
}

template<IsNotArithmetic T>
std::string getString(T &&value)
{
    if constexpr (Same<T, QString>)
        return value.toStdString();
    else if constexpr (Same<T, std::string>)
        return value;
    else if constexpr (Same<T, std::filesystem::path>)
        return value.string();
    else if constexpr (Same<T, QHostAddress>)
        return getString(value.toString());
    else
        return std::to_string(value);
}

template<IsOptional T>
std::string getString(T &value)
{
    return (value) ? getString(value.value()) : "EMPTY OPTIONAL";
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
        // std::make_format_args needs lvalue args
        auto formatArgs = std::make_format_args(params...);
        result = std::vformat(getString(std::forward<T>(formatString)), formatArgs);
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

template<typename T, typename... Ts>
void logTemplateException(T &&templateString, Ts &&...args);

template<AssociativeContainer M>
typename M::mapped_type getMapValue(
    const M &map, const typename M::key_type &key, const typename M::mapped_type &defaultValue)
{
    try {
        return map.at(key);
    }
    catch(std::out_of_range) {
        return defaultValue;
    }
    catch(const std::exception &e) {
        logTemplateException("{0} {1}", __FUNCTION__, e.what());

        return typename M::mapped_type();
    }
}

template<std::ranges::random_access_range R>
typename R::value_type getIndexedContainerValue(
    const R &container, std::size_t index, const typename R::value_type &defaultValue)
{
    try {
        return container.at(index);
    }
    catch(std::out_of_range) {
        return defaultValue;
    }
    catch(const std::exception &e) {
        logTemplateException("{0} {1}", __FUNCTION__, e.what());

        return typename R::value_type();
    }
}

template<std::ranges::bidirectional_range C>
typename C::value_type getNextItem(
    const C &container, const typename C::value_type &current,
    const typename C::value_type &defaultValue)
{
    if (container.empty()) {
        logTemplateError("{}, container is empty", __FUNCTION__);

        return defaultValue;
    }

    auto item = std::ranges::find(container, current);

    if (item == std::ranges::next(container.begin(), container.end())) {
        logTemplateError("{}, current item not found", __FUNCTION__);

        return defaultValue;
    }

    item = std::next(item);

    if (item == std::ranges::next(container.begin(), container.end()))
        item = container.begin();

    return *item;
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
        logTemplateException("{0} {1}", __FUNCTION__, e.what());
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
        logTemplateException("{0} {1}", __FUNCTION__, e.what());

        return false;
    }
}

template<typename F, typename... Ts>
bool runAndReturnTrue(F &&func, Ts&&... args)
{
    try {
        func(args...);
    }
    catch(const std::exception &e) {}

    return true;
}

template<typename F, typename... Ts>
std::optional<std::invoke_result_t<F, Ts...>> runAndReturnOptional(F &&func, Ts&&... args)
{
    try {
        return std::optional<std::invoke_result_t<F, Ts...>>(func(args...));
    }
    catch(const std::exception &e) {
        logTemplateException("{0} {1}", __FUNCTION__, e.what());

        return std::nullopt;
    }
}

template<typename F, typename... Ts>
std::invoke_result_t<F, Ts...> runAndReturnDefault(
    F &&func, const std::invoke_result_t<F, Ts...> &defaultValue, Ts&&... args)
{
    try {
        return func(args...);
    }
    catch(const std::exception &e) {
        logTemplateException("{0} {1}", __FUNCTION__, e.what());

        return defaultValue;
    }
}

template<typename F, typename... Ts>
std::invoke_result_t<F, Ts...> runAndReturnResultOrDefaultValue(
    F &&func, const std::invoke_result_t<F, Ts...> &defaultValue, Ts &&...args)
{
    try {
        return func(args...);
    }
    catch(const std::exception &e) {
        logTemplateException("{0} {1}", __FUNCTION__, e.what());

        return defaultValue;
    }
}

template<typename...Ts>
int getIntFromString(const int &defaultValue, Ts &&...args)
{
    auto lambda = [&](auto &&...args) { return std::stoi(args...); };

    return runAndReturnResultOrDefaultValue(lambda, defaultValue, args...);
}

template<typename F, typename... Ts>
std::invoke_result_t<F, Ts...> runAndReturnSharedPtr(F &&func, Ts&&... args)
{
    try {
        return func(args...);
    }
    catch(const std::exception &e) {
        logTemplateException("{0} {1}", __FUNCTION__, e.what());

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
        logTemplateException("{0} {1}", __FUNCTION__, e.what());
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

std::string buildTextAsUnorderedList(const std::vector<std::string> &messages);

std::string buildTextAsUnorderedListWithHeader(
    const std::string &header, const std::vector<std::string> &messages);

nlohmann::json buildJsonObject(const std::string &jsonString);
std::string getNetworkId();
std::string getRandomString(std::size_t length);
void sleepForMS(uint32_t duration);
bool getBoolFromDBValue(const std::string &value);
std::string hashFileMD5(const std::string &filename);
void exitUtil(int result);

std::optional<std::string> getValueFromEnvironmentVariable(std::string &&variableName);

#endif // UTILS_HPP
