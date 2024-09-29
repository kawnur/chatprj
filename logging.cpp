#include "logging.hpp"

template<> QString argForLogging<const std::string&>(const std::string& value)
{
    return QString::fromStdString(value);
}

QString argForLogging(const std::string& value)
{
    return QString::fromStdString(value);
}

QString argForLogging(const char* value)
{
    return QString(value);
}

QString argForLogging(const bool& value)
{
    return (value) ? QString("true") : QString("false");
}

void logLine(const QString& str)
{
    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->addTextToAppLogWidget(str);
}

void logLine(const std::string& str)
{
    MainWindow* mainWindow = getMainWindowPtr();
    mainWindow->addTextToAppLogWidget(QString::fromStdString(str));
}

