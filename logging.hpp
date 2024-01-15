#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <QString>
#include <string>
#include <type_traits>


#include "mainwindow.hpp"

class MainWindow;
MainWindow* getMainWindowPtr();

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

    ((text += (argForLogging(args) + QString(" "))), ...);

    mainWindow->addTextToRightPanel(text);
}

template<typename T> void logLine(const T& str) {
    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->addTextToRightPanel(QString(str));
}

void logLine(const QString&);
void logLine(const std::string&);


#endif // LOGGING_HPP
