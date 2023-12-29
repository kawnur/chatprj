cmake_minimum_required(VERSION 3.5)

project(chatprj LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
    application.cpp
    application.hpp
    async_tcp_echo_server.cpp  # tutorial boost asio
    async_tcp_echo_server.hpp  # tutorial boost asio
    blocking_tcp_echo_client.cpp  # tutorial boost asio
    blocking_tcp_echo_client.hpp  # tutorial boost asio
    blocking_tcp_echo_server.cpp  # tutorial boost asio
    blocking_tcp_echo_server.hpp  # tutorial boost asio
    build.py
    chat_client.cpp
    chat_client.hpp
    chat_server.cpp
    chat_server.hpp
    compose.yaml
    Dockerfile
    icmp_header.hpp  # tutorial boost asio
    ipv4_header.hpp  # tutorial boost asio
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
    tutorial_boost_asio.cpp  # tutorial boost asio
    tutorial_boost_asio.hpp  # tutorial boost asio
    utils_cout.cpp
    utils_cout.hpp
    utils_gpgme.cpp
    utils_gpgme.hpp
)

find_package(Boost REQUIRED COMPONENTS thread)
#find_package(Boost REQUIRED)

#find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)
#find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

#qt6_wrap_cpp(PROJECT_SOURCES mainwindow.hpp)

add_executable(chatapp ${PROJECT_SOURCES})

target_link_libraries(chatapp PRIVATE gpgme Boost::thread)
#target_link_libraries(chatapp PRIVATE gpgme Boost)
#target_link_libraries(chatapp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(chatapp PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
#target_link_libraries(chatapp PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
