#include "logging.hpp"

template<> QString argForLogging<const std::string&>(const std::string& value) {
    return QString::fromStdString(value);
}

QString argForLogging(const std::string& value) {
    return QString::fromStdString(value);
}

QString argForLogging(const char* value) {
    return QString(value);
}

void logLine(const QString& str) {
    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->addTextToRightPanel(str);
}

void logLine(const std::string& str) {
    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->addTextToRightPanel(QString::fromStdString(str));
}

