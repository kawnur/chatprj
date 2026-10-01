#include "utils_cout.hpp"

#include "logging.hpp"

void endline(int number)
{
    for (int i = 0; i < number; i++)
        std::cout << std::endl;
}

template<> int getSizeAsInt<std::string>(const std::string &string)
{
    return (int)string.size();
}

void coutVector1(const std::vector<int> &vector)
{
    std::cout << std::endl;

    for (auto &i : vector)
        std::cout << i << '\t';

    std::cout << std::endl;
}

void coutVectorAndSum(const std::vector<int> &vector)
{
    int sum = 0;

    std::cout << std::endl;

    for (auto &i : vector) {
        std::cout << i << '\t';
        sum += i;
    }

    std::cout << "sum: " << sum << '\t';
    std::cout << std::endl;
}

void coutVectorWithIndexesHorizontally(const std::vector<int> &vector)
{
    std::cout << std::endl;

    auto lambda = [&](const int &i, const int &j)
    {
        return std::to_string(i).size() < std::to_string(j).size();
    };

    auto maxElement = std::ranges::max_element(vector, lambda);

    int margin = 2;
    auto width = (int)std::to_string(*maxElement).size() + margin;

    coutWithManipulators("Index: ", width);

    for (int i = 0; i < (int)vector.size(); i++)
        coutWithManipulators(std::to_string(i), width);

    std::cout << std::endl;

    coutWithManipulators("Value: ", width);

    for (int i = 0; i < (int)vector.size(); i++)
        coutWithManipulators(std::to_string(vector.at(i)), width);

    std::cout << std::endl;
}

// array
void printArray(std::array<int, 10> &array)
{
    std::cout << "&a: " << &array << std::endl;

    for (std::size_t i = 0; i < array.size(); i++) {
        std::cout << "&a[" << i << "]: " << &array[i] << '\t';
        std::cout << "a[" << i << "]: " << array[i] << std::endl;
    }

    std::cout << "a.data(): " << array.data() << std::endl;
}

// tuple

// set
void printSet(std::set<int> &set)
{
    std::cout << std::endl << "container address: " << &set << std::endl;

    auto lambda = [&](const auto &item)
    {
        std::cout << "address: " << &item << '\t';
        std::cout << " value: " << item << std::endl;
    };

    std::ranges::for_each(set, lambda);

    std::cout << "container size: " << set.size() << std::endl;
}

// map

// string
void coutString(std::string::const_iterator iterator1, std::string::const_iterator iterator2)
{
    for (auto it = iterator1; it != iterator2; it++)
        std::cout << *it;

    endline(1);
}

void coutStringFull(std::string &string)
{
    coutString(string.begin(), string.end());
}

void coutHeader(const std::string &header)
{
    char marginSymbol = '#';
    char spaceSymbol = ' ';
    int marginSize = 5;
    int spaceSize = 3;

    auto headerSize = marginSize + spaceSize + header.size() + spaceSize + marginSize;
    std::string symbolString(headerSize, marginSymbol);
    auto preHeader =std::string(marginSize, marginSymbol) + std::string(spaceSize, spaceSymbol);
    auto postHeader = std::string(spaceSize, spaceSymbol) + std::string(marginSize, marginSymbol);

    endline(1);
    coutWithEndl(symbolString);
    coutWithEndl(preHeader + header + postHeader);
    coutWithEndl(symbolString);
    endline(1);
}
