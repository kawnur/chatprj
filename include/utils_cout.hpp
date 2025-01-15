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

//void coutWithManipulators(auto&& s, int width) {
template<typename T> void coutWithManipulators(T&& s, int width)
{
	std::cout << std::left << std::setw(width) << std::setfill(' ') << s;
}

//void coutWithManipulatorsLeft(auto&& s, int width) {
template<typename T> void coutWithManipulatorsLeft(T&& s, int width)
{
	std::cout << std::left << std::setw(width) << std::setfill(' ') << s;
}

//void coutWithManipulatorsRight(auto&& s, int width) {
template<typename T> void coutWithManipulatorsRight(T&& s, int width)
{
	std::cout << std::right << std::setw(width) << std::setfill(' ') << s;
}

template<typename... Ts> void coutArgsWithManipulators(int width, Ts&&... args)
{
	(coutWithManipulators(args, width), ...);
	endline(1);
}

template<typename U> int getSizeAsInt(U& u)
{
	return (int)std::to_string(u).size();
}


// TODO doesnot work with multiple function pointers as params
//template<typename U, typename... Ts> void coutContainerWithManupulators(std::vector<U>& container, Ts&&... args)
template<typename U, typename... Ts> void coutContainerArgsResultWithManupulators(std::vector<U>& container, Ts&& ... args...)
//template<typename U> void coutContainerArgsResultWithManupulators(std::vector<U>& container, U(*)(U)...args)
{
    auto check = [&](U& u1, U& u2)
    {
		return getSizeAsInt(u1) < getSizeAsInt(u2);
	};

	auto maxLengthElement = std::max_element(container.begin(), container.end(), check);
	int width = (int)(maxLengthElement->size());

    for(auto& element : container)
    {
		coutArgsWithManipulators(width, (args(element), ...));
	}
}

//	template<typename T, std::ios_base& (*alignment)(), int width, char separator> void coutWithManipulators(T& t)
//	template<typename T, std::function<std::ios_base&(std::ios_base&)> alignment(std::ios_base&), int width, char separator> void coutWithManipulators(T& t)
//	template<typename T, decltype(std::left) alignment, int width, char separator> void coutWithManipulators(T&& t)
template<typename T, std::ios_base& (*alignment)(std::ios_base&), int width, char separator>
void coutWithManipulators(T&& t)
{
	std::cout << alignment << std::setw(width) << std::setfill(separator) << t;
}

template<typename T, std::ios_base& (*alignment)(std::ios_base&), int width, char separator>
void coutWithManipulators(T& t)
{
	std::cout << alignment << std::setw(width) << std::setfill(separator) << t;
}

template<typename T> void coutWithEndl(T&& t)
{
	std::cout << t << std::endl;
}

template<typename T> void coutStringWithEndl(T&& t)
{
    if(t == nullptr)
    {
		coutWithEndl("nullptr");
	}
    else
    {
		coutWithEndl(std::string(t));
	}
}

template<typename T> void coutWithTab(T&& t)
{
	std::cout << t << '\t';
}

template<typename T> void coutWithoutInterval(T&& t)
{
	std::cout << t;
}

template<typename T> std::string argForCout(T* const& value)
{
    std::stringstream ss;
    ss << (void*)value;
    return ss.str();
}

template<typename T> std::string argForCout(const T& value)
{
    return std::to_string(value);
}

std::string argForCout(const QString&);
std::string argForCout(const std::string&);
std::string argForCout(const char*);

template<typename... Ts> void coutArgsWithSeparator(char separator, Ts&&... args)
{
//	(std::cout << ... << args) << std::endl;
//	auto l = [](auto& i){ std::cout << i << separator; }
//	(l << ... << args) << std::endl;
//	((std::cout << '\t')... << args) << std::endl;
//	(l(args), ...);

//    ((std::cout << args << separator), ...);
    ((std::cout << argForCout(args) << separator), ...);

//	([&]{ std::cout << '\t' << i; }(), ...);
//	(std::cout (<< '\t' <<) ... (<< '\t' <<) args) << std::endl;
	endline(1);
}

template<typename... Ts> void coutArgsWithSpaceSeparator(Ts&&... args)
{
	coutArgsWithSeparator(' ', args...);
}

template<typename... Ts> void coutArgsWithTabSeparator(Ts&&... args)
{
	coutArgsWithSeparator('\t', args...);
}


// cout vector
template<typename T> void coutVectorInLine(const std::vector<T>& v)
{
    for(auto& i : v)
    {
		std::cout << i << ' ';
	}
}

template<typename T> void printVector(T& v, bool showSize = false, bool showCapacity = false)
{
	std::cout << std::endl << "printVector" << std::endl;

    for(std::size_t i = 0; i < v.size(); i++)
    {
		std::cout << v.at(i) << " ";
	}
	endline(1);

    if(showSize)
    {
		std::cout << "v.size(): " << v.size() << std::endl;
	}
    if(showCapacity)
    {
		std::cout << "v.capacity(): " << v.capacity() << std::endl;
	}
	endline(1);
}

template<typename T> void coutVectorState(T& v)
{
	endline(1);
	coutWithEndl("coutVectorState");

	int width = 20;
	int width1 = 11;

	coutArgsWithManipulators(width, "ADDRESS", "INDEX", "VALUE");

    for(std::size_t i = 0; i < v.size(); i++)
    {
		coutArgsWithManipulators(width, &(v.at(i)), i, v.at(i));
	}

	coutArgsWithManipulators(width1, "SIZE:", v.size());
	coutArgsWithManipulators(width1, "CAPACITY:", v.capacity());
	endline(1);
}

template<typename T> void coutVectorOfVectorsState(T& v)
{
	endline(1);
	coutWithEndl("coutVectorState");

    for(auto iter = v.begin(); iter != v.end(); iter++)
    {
		coutWithTab(&*iter);
		coutVectorInLine(*iter);
		endline(1);
	}
	coutWithTab(&*(v.end()));
	coutWithEndl("end");

	endline(1);

	coutArgsWithSeparator(' ', "v.size():", v.size());
	coutArgsWithSeparator(' ', "v.capacity():", v.capacity());
	endline(2);
}

void coutVector1(const std::vector<int>&);
void coutVectorAndSum(const std::vector<int>&);
void coutVectorWithIndexesHorizontally(const std::vector<int>&);


// array
void printArray(std::array<int, 10>&);


// tuple
template<typename T, std::size_t... I> void printTuple(T&& a)
{
//		std::cout << "a.get<1>(a): " << std::get<0>(a) << '\t';
//		std::cout << "a.get<2>(a): " << std::get<1>(a) << std::endl;

//    	for(int i = 0; i < s; i++) {
//    		std::cout << "a.get<" << s << ">(a): " << std::get<s>(a) << std::endl;
//    	}
	([](auto a){ std::cout << "a.get<" << I << ">(a): " << std::get<I>(a) << std::endl; }, ...);
}


// set
void printSet(std::set<int>&);


// map
template<typename T, typename U> void coutMap(std::map<T, U>& m)
{
    for(auto it = m.begin(); it != m.end(); it++)
    {
		coutArgsWithManipulators(15, it->first, it->second);
	}
	endline(1);
}


// string
void coutString(std::string::const_iterator, std::string::const_iterator);
void coutStringFull(std::string&);
void coutHeader(const std::string&);


#endif
