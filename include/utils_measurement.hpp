#ifndef UTILS_MEASUREMENT_HPP
#define UTILS_MEASUREMENT_HPP

#include <chrono>

#include "utils_cout.hpp"

template<typename T, typename F, typename... Ts>
T runMeasureDurationAndReturnResult(F&& func, Ts... params) {
    auto begin = std::chrono::high_resolution_clock::now();

    auto result = func(params...);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    auto elapsedCount = elapsed.count();
    coutArgsWithSpaceSeparator("elapsed:", elapsedCount, "nanoseconds");

    return result;
}

template<typename F, typename... Ts>
void runAndMeasureDuration(F&& func, Ts... params) {
    auto begin = std::chrono::high_resolution_clock::now();

    func(params...);

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin);

    auto elapsedCount = elapsed.count();
    coutArgsWithSpaceSeparator("elapsed:", elapsedCount, "nanoseconds");
}

#endif // UTILS_MEASUREMENT_HPP
