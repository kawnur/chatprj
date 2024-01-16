cmake_minimum_required(VERSION 3.5)

project(chatprj LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
    .env
    alternative_realisations
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
    db_interaction.cpp
    db_interaction.hpp
    sql_scripts/sql_script_1_configure_db.sql
    sql_scripts/client1_db/sql_script_2_create_data_client1_db.sql
    sql_scripts/client2_db/sql_script_2_create_data_client2_db.sql
    sql_scripts/client3_db/sql_script_2_create_data_client3_db.sql
    Dockerfile
    icmp_header.hpp  # tutorial boost asio
    ipv4_header.hpp  # tutorial boost asio
    logging.cpp
    logging.hpp
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
    manager.cpp
    manager.hpp
    tutorial_boost_asio.cpp  # tutorial boost asio
    tutorial_boost_asio.hpp  # tutorial boost asio
    utils_cout.cpp
    utils_cout.hpp
    utils_gpgme.cpp
    utils_gpgme.hpp
    widgets.cpp
    widgets.hpp
)

include(FindPostgreSQL)

#message("PostgreSQL_FOUND " ${PostgreSQL_FOUND})
#message("PostgreSQL_LIBRARIES " ${PostgreSQL_LIBRARIES})
#message("PostgreSQL_INCLUDE_DIRS " ${PostgreSQL_INCLUDE_DIRS})
#message("PostgreSQL_LIBRARY_DIRS " ${PostgreSQL_LIBRARY_DIRS})
#message("PostgreSQL_VERSION_STRING " ${PostgreSQL_VERSION_STRING})
#message("PostgreSQL_TYPE_INCLUDE_DIR " ${PostgreSQL_TYPE_INCLUDE_DIR})

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
target_link_libraries(chatapp PRIVATE pq)

# copy sql scripts

file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client1_db)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client2_db)
file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client3_db)

file(COPY_FILE
    ${PROJECT_SOURCE_DIR}/sql_scripts/sql_script_1_configure_db.sql
    ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client1_db/sql_script_1_configure_db.sql)

file(COPY_FILE
    ${PROJECT_SOURCE_DIR}/sql_scripts/sql_script_1_configure_db.sql
    ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client2_db/sql_script_1_configure_db.sql)

file(COPY_FILE
    ${PROJECT_SOURCE_DIR}/sql_scripts/sql_script_1_configure_db.sql
    ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client3_db/sql_script_1_configure_db.sql)

file(COPY_FILE
    ${PROJECT_SOURCE_DIR}/sql_scripts/client1_db/sql_script_2_create_data_client1_db.sql
    ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client1_db/sql_script_2_create_data_client1_db.sql)

file(COPY_FILE
    ${PROJECT_SOURCE_DIR}/sql_scripts/client2_db/sql_script_2_create_data_client2_db.sql
    ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client2_db/sql_script_2_create_data_client2_db.sql)

file(COPY_FILE
    ${PROJECT_SOURCE_DIR}/sql_scripts/client3_db/sql_script_2_create_data_client3_db.sql
    ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client3_db/sql_script_2_create_data_client3_db.sql)
