#include "logging.hpp"

template<> QString argForLogging(const std::string& str) {
    return QString::fromStdString(str);
}

void logLine(const QString& str) {
    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->addTextToRightPanel(str);
}

void logLine(const std::string& str) {
    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->addTextToRightPanel(QString::fromStdString(str));
}

