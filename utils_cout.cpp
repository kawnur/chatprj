#include "utils_cout.hpp"


void endline(int n) {
    for(int i = 0; i < n; i++) {
        std::cout << std::endl;
    }
}

template<> int getSizeAsInt<std::string>(std::string& s) {
    return (int)s.size();
}

std::string argForCout(const QString& value) {
    return value.toStdString();
}

std::string argForCout(const std::string& value) {
    return value;
}

std::string argForCout(const char* value) {
    return std::string(value);
}

void coutVector1(const std::vector<int>& v) {
    std::cout << std::endl;

    for(auto& i : v) {
        std::cout << i << '\t';
    }

    std::cout << std::endl;
};

void coutVectorAndSum(const std::vector<int>& v) {
    int sum = 0;

    std::cout << std::endl;

    for(auto& i : v) {
        std::cout << i << '\t';
        sum += i;
    }

    std::cout << "sum: " << sum << '\t';
    std::cout << std::endl;
};

void coutVectorWithIndexesHorizontally(const std::vector<int>& v) {
    std::cout << std::endl;

    auto compareStringLengths = [&](const int& i, const int& j) {
        return std::to_string(i).size() < std::to_string(j).size();
    };

    auto maxElement = std::max_element(v.begin(), v.end(), compareStringLengths);
//		std::cout << "maxElement: " << *maxElement << std::endl;

    int margin = 2;
    auto width = (int)std::to_string(*maxElement).size() + margin;

    coutWithManipulators("Index: ", width);

    for(int i = 0; i < (int)v.size(); i++) {
        coutWithManipulators(std::to_string(i), width);
    }

    std::cout << std::endl;

    coutWithManipulators("Value: ", width);

    for(int i = 0; i < (int)v.size(); i++) {
        coutWithManipulators(std::to_string(v.at(i)), width);
    }

    std::cout << std::endl;
};


// array

void printArray(std::array<int, 10>& a) {
    std::cout << "&a: " << &a << std::endl;

    for(size_t i = 0; i < a.size(); i++) {
        std::cout << "&a[" << i << "]: " << &a[i] << '\t';
        std::cout << "a[" << i << "]: " << a[i] << std::endl;
    }
    std::cout << "a.data(): " << a.data() << std::endl;
}


// tuple


// set

void printSet(std::set<int>& s) {
    std::cout << std::endl << "&s: " << &s << std::endl;
    for(std::set<int>::iterator i = s.cbegin(); i != s.cend(); i++) {
        std::cout << "&(*i): " << &(*i) << '\t';
        std::cout << " *i: " << *i << std::endl;
    }
    std::cout << "s.size(): " << s.size() << std::endl;
}


// map


// string
void coutString(std::string::const_iterator it1, std::string::const_iterator it2) {
    for(auto it = it1; it != it2; it++) {
        std::cout << *it;
    }
    endline(1);
}

void coutStringFull(std::string& s) {
    coutString(s.begin(), s.end());
}

void coutHeader(const std::string& header) {
    char marginSymbol = '#';
    char spaceSymbol = ' ';
    int marginSize = 5;
    int spaceSize = 3;
    auto headerSize = marginSize + spaceSize + header.size() + spaceSize + marginSize;

    std::string symbolString(headerSize, marginSymbol);
    std::string preHeader = std::string(marginSize, marginSymbol) + std::string(spaceSize, spaceSymbol);
    std::string postHeader = std::string(spaceSize, spaceSymbol) + std::string(marginSize, marginSymbol);

    endline(1);
    coutWithEndl(symbolString);
    coutWithEndl(preHeader + header + postHeader);
    coutWithEndl(symbolString);
    endline(1);
}
