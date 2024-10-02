#ifndef UTILS_HPP
#define UTILS_HPP

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

#endif // UTILS_HPP
