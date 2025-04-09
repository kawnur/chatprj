#ifndef UTILS_COUT_HPP
#define UTILS_COUT_HPP

#include <algorithm>
#include <array>
#include <iomanip>
#include <iosfwd>
#include <iostream>
#include <QString>
#include <map>
#include <set>
#include <type_traits>
#include <vector>

void endline(int);

template<typename T> void coutWithManipulators(T&& value, int width) {
    std::cout << std::left << std::setw(width) << std::setfill(' ') << value;
}

template<typename T> void coutWithManipulatorsLeft(T&& value, int width) {
    std::cout << std::left << std::setw(width) << std::setfill(' ') << value;
}

template<typename T> void coutWithManipulatorsRight(T&& value, int width) {
    std::cout << std::right << std::setw(width) << std::setfill(' ') << value;
}

template<typename... Ts> void coutArgsWithManipulators(int width, Ts&&... args) {
	(coutWithManipulators(args, width), ...);
	endline(1);
}

template<typename T> int getSizeAsInt(T& parameter) {
    return (int)std::to_string(parameter).size();
}

// TODO does not work with multiple function pointers as params
template<typename T, typename... Ts>
void coutContainerArgsResultWithManupulators(
    std::vector<T>& container, Ts&&... args) {
    auto check = [&](T& u1, T& u2) {
		return getSizeAsInt(u1) < getSizeAsInt(u2);
	};

    auto maxLengthElement = std::max_element(
        container.begin(), container.end(), check);

	int width = (int)(maxLengthElement->size());

    for(auto& element : container) {
		coutArgsWithManipulators(width, (args(element), ...));
	}
}

template<
    typename T, std::ios_base& (*alignment)(std::ios_base&),
    int width, char separator>
void coutWithManipulators(T&& value) {
    std::cout << alignment << std::setw(width)
              << std::setfill(separator) << std::forward<T>(value);
}

template<typename T> void coutWithEndl(T&& value) {
    std::cout << value << std::endl;
}

template<typename T> void coutWithTab(T&& value) {
    std::cout << value << '\t';
}

template<typename T> void coutWithoutInterval(T&& value) {
    std::cout << value;
}

template<typename T> std::string argForCout(T* const& value) {
    std::stringstream ss;
    ss << (void*)value;
    return ss.str();
}

template<typename T> std::string argForCout(const T& value) {
    return std::to_string(value);
}

std::string argForCout(const QString&);
std::string argForCout(const std::string&);
std::string argForCout(const char*);

template<typename... Ts> void coutArgsWithSeparator(char separator, Ts&&... args) {
    ((std::cout << argForCout(args) << separator), ...);
	endline(1);
}

template<typename... Ts> void coutArgsWithSpaceSeparator(Ts&&... args) {
	coutArgsWithSeparator(' ', args...);
}

template<typename... Ts> void coutArgsWithTabSeparator(Ts&&... args) {
	coutArgsWithSeparator('\t', args...);
}

// cout vector
template<typename T> void coutVectorInLine(const std::vector<T>& vector) {
    for(auto& item : vector) {
        std::cout << item << ' ';
	}
}

template<typename T>
void printVector(T& vector, bool showSize = false, bool showCapacity = false) {
	std::cout << std::endl << "printVector" << std::endl;

    for(std::size_t i = 0; i < vector.size(); i++) {
        std::cout << vector.at(i) << " ";
	}
	endline(1);

    if(showSize) {
        std::cout << "v.size(): " << vector.size() << std::endl;
	}
    if(showCapacity) {
        std::cout << "v.capacity(): " << vector.capacity() << std::endl;
	}
	endline(1);
}

template<typename T> void coutVectorState(T& vector) {
	endline(1);
	coutWithEndl("coutVectorState");

	int width = 20;
	int width1 = 11;

	coutArgsWithManipulators(width, "ADDRESS", "INDEX", "VALUE");

    for(std::size_t i = 0; i < vector.size(); i++) {
        coutArgsWithManipulators(width, &(vector.at(i)), i, vector.at(i));
	}

    coutArgsWithManipulators(width1, "SIZE:", vector.size());
    coutArgsWithManipulators(width1, "CAPACITY:", vector.capacity());
	endline(1);
}

template<typename T> void coutVectorOfVectorsState(T& vector) {
	endline(1);
	coutWithEndl("coutVectorState");

    for(auto iter = vector.begin(); iter != vector.end(); iter++) {
		coutWithTab(&*iter);
		coutVectorInLine(*iter);
		endline(1);
	}
    coutWithTab(&*(vector.end()));
	coutWithEndl("end");

	endline(1);

    coutArgsWithSeparator(' ', "v.size():", vector.size());
    coutArgsWithSeparator(' ', "v.capacity():", vector.capacity());
	endline(2);
}

void coutVector1(const std::vector<int>&);
void coutVectorAndSum(const std::vector<int>&);
void coutVectorWithIndexesHorizontally(const std::vector<int>&);

// array
void printArray(std::array<int, 10>&);

// tuple
template<typename T, std::size_t... I> void printTuple(T&& container) {
    (
        [](auto container) {
            std::cout << "a.get<" << I << ">(a): "
                      << std::get<I>(container) << std::endl;
        },
        ...);
}

// set
void printSet(std::set<int>&);

// map
template<typename T, typename U> void coutMap(std::map<T, U>& map) {
    for(auto it = map.begin(); it != map.end(); it++) {
		coutArgsWithManipulators(15, it->first, it->second);
	}
	endline(1);
}

// string
void coutString(std::string::const_iterator, std::string::const_iterator);
void coutStringFull(std::string&);
void coutHeader(const std::string&);

#endif
