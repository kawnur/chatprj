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

void logLine(const QString& string)
{
    // MainWindow* mainWindowPtr = getMainWindowPtr();
    // mainWindowPtr->addTextToAppLogWidget(str);
    getGraphicManagerPtr()->addTextToAppLogWidget(string);
}

void logLine(const std::string& string)
{
    // MainWindow* mainWindowPtr = getMainWindowPtr();
    // mainWindowPtr->addTextToAppLogWidget(QString::fromStdString(string));
    getGraphicManagerPtr()->addTextToAppLogWidget(QString::fromStdString(string));
}

