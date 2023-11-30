cmake_minimum_required(VERSION 3.5)

project(chatprj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
    build.py
    compose.yaml
    Dockerfile
    main.cpp
    tutorial_boost_asio.cpp
    tutorial_boost_asio.hpp
    utils_cout.cpp
    utils_cout.hpp
    utils_gpgme.cpp
    utils_gpgme.hpp
)

find_package(Boost REQUIRED COMPONENTS thread)
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_executable(chatapp ${PROJECT_SOURCES})

target_link_libraries(chatapp PRIVATE gpgme Boost::thread)
target_link_libraries(chatapp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
