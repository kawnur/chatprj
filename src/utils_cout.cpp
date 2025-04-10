#include "utils_cout.hpp"

void endline(int number) {
    for(int i = 0; i < number; i++) {
        std::cout << std::endl;
    }
}

template<> int getSizeAsInt<std::string>(std::string& string) {
    return (int)string.size();
}

std::string argForCout(const QString& value) {
    return value.toStdString();
}

std::string argForCout(const std::string& value) {
    return value;
}

std::string argForCout(const char* value) {
    return (value) ? std::string(value) : std::string("0x0");
}

void coutVector1(const std::vector<int>& vector) {
    std::cout << std::endl;

    for(auto& i : vector) {
        std::cout << i << '\t';
    }

    std::cout << std::endl;
}

void coutVectorAndSum(const std::vector<int>& vector) {
    int sum = 0;

    std::cout << std::endl;

    for(auto& i : vector) {
        std::cout << i << '\t';
        sum += i;
    }

    std::cout << "sum: " << sum << '\t';
    std::cout << std::endl;
}

void coutVectorWithIndexesHorizontally(const std::vector<int>& vector) {
    std::cout << std::endl;

    auto compareStringLengths = [&](const int& i, const int& j){
        return std::to_string(i).size() < std::to_string(j).size();
    };

    auto maxElement =
        std::max_element(vector.begin(), vector.end(), compareStringLengths);

    int margin = 2;
    auto width = (int)std::to_string(*maxElement).size() + margin;

    coutWithManipulators("Index: ", width);

    for(int i = 0; i < (int)vector.size(); i++) {
        coutWithManipulators(std::to_string(i), width);
    }

    std::cout << std::endl;

    coutWithManipulators("Value: ", width);

    for(int i = 0; i < (int)vector.size(); i++) {
        coutWithManipulators(std::to_string(vector.at(i)), width);
    }

    std::cout << std::endl;
}

// array
void printArray(std::array<int, 10>& array) {
    std::cout << "&a: " << &array << std::endl;

    for(std::size_t i = 0; i < array.size(); i++) {
        std::cout << "&a[" << i << "]: " << &array[i] << '\t';
        std::cout << "a[" << i << "]: " << array[i] << std::endl;
    }
    std::cout << "a.data(): " << array.data() << std::endl;
}

// tuple

// set
void printSet(std::set<int>& set) {
    std::cout << std::endl << "&s: " << &set << std::endl;

    for(std::set<int>::iterator i = set.cbegin(); i != set.cend(); i++) {
        std::cout << "&(*i): " << &(*i) << '\t';
        std::cout << " *i: " << *i << std::endl;
    }
    std::cout << "s.size(): " << set.size() << std::endl;
}

// map

// string
void coutString(
    std::string::const_iterator iterator1, std::string::const_iterator iterator2) {
    for(auto it = iterator1; it != iterator2; it++) {
        std::cout << *it;
    }
    endline(1);
}

void coutStringFull(std::string& string) {
    coutString(string.begin(), string.end());
}

void coutHeader(const std::string& header) {
    char marginSymbol = '#';
    char spaceSymbol = ' ';
    int marginSize = 5;
    int spaceSize = 3;
    auto headerSize = marginSize + spaceSize + header.size() + spaceSize + marginSize;

    std::string symbolString(headerSize, marginSymbol);

    std::string preHeader =
        std::string(marginSize, marginSymbol) + std::string(spaceSize, spaceSymbol);

    std::string postHeader =
        std::string(spaceSize, spaceSymbol) + std::string(marginSize, marginSymbol);

    endline(1);
    coutWithEndl(symbolString);
    coutWithEndl(preHeader + header + postHeader);
    coutWithEndl(symbolString);
    endline(1);
}
