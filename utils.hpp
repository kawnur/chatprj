#ifndef UTILS_HPP
#define UTILS_HPP

#include <QHostAddress>

#include "logging.hpp"

template<typename... Ts> void logArgsException(Ts&&... args);

template<typename F, typename... Ts>
void runAndLogException(F func, Ts&&... args)
{
    try
    {
        func(args...);
    }
    catch(std::exception& e)
    {
        logArgsException(e.what());
    }
}

// data validation

bool validateCompanionName(const std::string&);
bool validateIpAddress(const std::string&);
bool validatePort(const std::string&);

bool validateCompanionData(
    std::vector<std::string>&, const std::string&,
    const std::string&, const std::string&);

#endif // UTILS_HPP
