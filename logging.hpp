#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <QString>
#include <string>

#include "mainwindow.hpp"

class MainWindow;
MainWindow* getMainWindowPtr();

template<typename T> QString argForLogging(const T& str) {
    return QString(str);
}

template<> QString argForLogging(const std::string&);

template<typename... Ts> void logArgs(Ts&&... args) {
    MainWindow* mainWindow = getMainWindowPtr();
    QString text;

    ((text += argForLogging(args)), ...);

    mainWindow->addTextToRightPanel(text);
}

template<typename T> void logLine(const T& str) {
    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->addTextToRightPanel(QString(str));
}

void logLine(const QString&);
void logLine(const std::string&);


#endif // LOGGING_HPP
