cmake_minimum_required(VERSION 3.5)

project(chatprj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
    main.cpp
    utils_cout.cpp
    utils_cout.hpp
    utils_gpgme.cpp
    utils_gpgme.hpp
)

add_executable(chatprj ${PROJECT_SOURCES})

target_link_libraries(chatprj gpgme)
