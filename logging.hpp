#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <QString>
#include <string>
#include <type_traits>

#include "mainwindow.hpp"
#include "utils_cout.hpp"

class MainWindow;
MainWindow* getMainWindowPtr();

template<typename T> QString argForLogging(T* const& value) {
    std::stringstream ss;
    ss << (void*)value;
    return QString::fromStdString(ss.str());
}

template<
        typename T,
        typename std::enable_if_t<std::is_arithmetic_v<T>, bool> = true>
QString argForLogging(const T& value) {
    return QString::fromStdString(std::to_string(value));
}

template<
        typename T,
        typename std::enable_if_t<!std::is_arithmetic_v<T>, bool> = true>
QString argForLogging(const T& value) {
    return QString(value);
}

QString argForLogging(const std::string&);
QString argForLogging(const char*);

template<typename... Ts> void logArgs(Ts&&... args) {
    MainWindow* mainWindow = getMainWindowPtr();
    QString text;

    text += QString("- ");

    ((text += (argForLogging(args) + QString(" "))), ...);

    mainWindow->addTextToAppLogWidget(text);
    coutArgsWithSpaceSeparator(args...);
}

template<typename T> void logLine(const T& str) {
    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->addTextToAppLogWidget(QString(str));
}

void logLine(const QString&);
void logLine(const std::string&);


#endif // LOGGING_HPP
