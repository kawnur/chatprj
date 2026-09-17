#ifndef UTILS_COUT_HPP
#define UTILS_COUT_HPP

#include <algorithm>
#include <iomanip>
#include <ios>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <QString>

#include "utils.hpp"

using namespace std::string_literals;

void endline(int number);

template<typename T>
void coutWithManipulators(T &&value, int width)
{
    std::cout << std::left << std::setw(width) << std::setfill(' ') << value;
}

template<typename T>
void coutWithManipulatorsLeft(T &&value, int width)
{
    std::cout << std::left << std::setw(width) << std::setfill(' ') << value;
}

template<typename T>
void coutWithManipulatorsRight(T &&value, int width)
{
    std::cout << std::right << std::setw(width) << std::setfill(' ') << value;
}

template<typename... Ts>
void coutArgsWithManipulators(int width, Ts &&...args)
{
	(coutWithManipulators(args, width), ...);
	endline(1);
}

template<typename T>
int getSizeAsInt(const T &parameter)
{
    return (int)std::to_string(parameter).size();
}

template<>
int getSizeAsInt<std::string>(const std::string &string);

// TODO does not work with multiple function pointers as params
template<typename T, typename... Ts>
void coutContainerArgsResultWithManupulators(std::vector<T> &container, Ts &&...args)
{
    auto lambda = [&](const T &u1, const T &u2) { return getSizeAsInt(u1) < getSizeAsInt(u2); };
    auto maxLengthElement = std::ranges::max_element(container, lambda);
	int width = (int)(maxLengthElement->size());

    for (auto &element : container)
        coutArgsWithManipulators(width, (args(element), ...));
}

template<typename T, std::ios_base &(*alignment)(std::ios_base &), int width, char separator>
void coutWithManipulators(T &&value)
{
    std::cout << alignment << std::setw(width)
              << std::setfill(separator) << std::forward<T>(value);
}

template<typename T>
std::string argForCout(T *&value)
{
    std::stringstream ss;
    ss << (void *)value;

    return ss.str();
}

template<typename T>
std::string argForCout(std::shared_ptr<T> value)
{
    return argForCout(*value);
}

template<typename T>
std::string argForCout(const T &value)
{
    return std::to_string(value);
}

std::string argForCout(const QString &value);
std::string argForCout(const std::string &value);
std::string argForCout(const char *value);
std::string argForCout(const bool value);

template<typename T>
void coutWithEndl(T &&value)
{
    std::cout << argForCout(value) << std::endl;
}

template<typename T>
void coutWithTab(T &&value)
{
    std::cout << argForCout(value) << '\t';
}

template<typename T>
void coutWithSpace(T &&value)
{
    std::cout << argForCout(value) << ' ';
}

template<typename T>
void coutWithoutInterval(T &&value)
{
    std::cout << argForCout(value);
}

template<typename... Ts>
void coutArgsWithSeparator(char separator, Ts&&... args)
{
    ((std::cout << argForCout(args) << separator), ...);
	endline(1);
}

template<typename... Ts>
void coutArgsWithSpaceSeparator(Ts&&... args)
{
	coutArgsWithSeparator(' ', args...);
}

template<typename... Ts>
void coutArgsWithTabSeparator(Ts&&... args)
{
	coutArgsWithSeparator('\t', args...);
}

// cout vector
template<typename T>
void coutVectorInLine(const std::vector<T> &vector)
{
    std::ranges::for_each(vector, [&](const auto &item){ std::cout << item << ' '; });
}

template<typename T>
void printVector(T &vector, bool showSize = false, bool showCapacity = false)
{
    std::cout << std::endl << __FUNCTION__ << std::endl;

    std::ranges::for_each(vector, [&](const auto &item){ std::cout << item << ' '; });

    endline(1);

    if (showSize)
        std::cout << "v.size(): " << vector.size() << std::endl;

    if (showCapacity)
        std::cout << "v.capacity(): " << vector.capacity() << std::endl;

	endline(1);
}

template<typename T>
void coutVectorState(const T &vector)
{
	endline(1);
    coutWithEndl(__FUNCTION__);

    coutArgsWithSpaceSeparator("vector object address:", &vector);

	int width = 20;
	int width1 = 11;

	coutArgsWithManipulators(width, "ADDRESS", "INDEX", "VALUE");

    for (std::size_t i = 0; i < vector.size(); i++)
        coutArgsWithManipulators(width, &(vector.at(i)), i, vector.at(i));

    coutArgsWithManipulators(width1, "SIZE:", vector.size());
    coutArgsWithManipulators(width1, "CAPACITY:", vector.capacity());
	endline(1);
}

template<typename T>
void coutVectorOfVectorsState(T &vector)
{
	endline(1);
	coutWithEndl("coutVectorState");


    auto lambda = [&](const auto &item)
    {
        coutWithTab(&*item);
        coutVectorInLine(*item);
		endline(1);
    };

    std::ranges::for_each(vector, lambda);

    coutWithTab(&*(vector.end()));
	coutWithEndl("end");
	endline(1);
    coutArgsWithSeparator(' ', "v.size():", vector.size());
    coutArgsWithSeparator(' ', "v.capacity():", vector.capacity());
	endline(2);
}

void coutVector1(const std::vector<int> &vector);
void coutVectorAndSum(const std::vector<int> &vector);
void coutVectorWithIndexesHorizontally(const std::vector<int> &vector);

// TODO rewrite functions below with ranges

// array
void printArray(std::array<int, 10> &array);

// tuple
// template<typename T, std::size_t... I>
// void printTuple(T &&container)
// {
//     auto lambda = [](auto container)
//     {
//         std::cout << "a.get<" << I << ">(a): " << std::get<I>(container) << std::endl;
//     };

//     (lambda, ...);
// }

// set
void printSet(std::set<int> &set);

// map
template<typename T, typename U>
void coutMap(std::map<T, U> &map)
{
    auto lambda = [&](const auto &item)
    {
        coutArgsWithManipulators(15, item->first, item->second);
    };

    std::ranges::for_each(map, lambda);

	endline(1);
}

template<typename T, typename U>
void coutMappingValue(const std::map<T, U> &map, const T &key)
{
    auto line = getMapValue(map, key, ""s);

    if (!line.empty())
        coutWithEndl(line);
}

// string
void coutString(std::string::const_iterator iterator1, std::string::const_iterator iterator2);
void coutStringFull(std::string &string);
void coutHeader(const std::string &header);

#endif
