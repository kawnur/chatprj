cmake_minimum_required(VERSION 3.5)

project(chatprj LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(PROJECT_SOURCES
    action.cpp
    action.hpp
    application.cpp
    application.hpp
    chat_client.cpp
    chat_client.hpp
    chat_server.cpp
    chat_server.hpp
    companion.cpp
    companion.hpp
    constants.hpp
    data.cpp
    data.hpp
    db_interaction.cpp
    db_interaction.hpp
    graphic_manager.cpp
    graphic_manager.hpp
    logging.cpp
    logging.hpp
    main.cpp
    mainwindow.cpp
    mainwindow.hpp
    manager.cpp
    manager.hpp
    message.cpp
    message.hpp
    utils.cpp
    utils.hpp
    utils_cout.cpp
    utils_cout.hpp
    utils_gpgme.cpp
    utils_gpgme.hpp
    utils_widgets.cpp
    utils_widgets.hpp
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
find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Widgets)
#find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)
#find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

find_package(nlohmann_json 3.2.0 REQUIRED)

#qt6_wrap_cpp(PROJECT_SOURCES mainwindow.hpp)

add_executable(chatapp ${PROJECT_SOURCES})

target_link_libraries(chatapp PRIVATE gpgme Boost::thread)
#target_link_libraries(chatapp PRIVATE gpgme Boost)
#target_link_libraries(chatapp PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(chatapp PRIVATE Qt6::Core Qt6::Gui Qt6::Network Qt6::Widgets)
#target_link_libraries(chatapp PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Sql)
target_link_libraries(chatapp PRIVATE pq)
target_link_libraries(chatapp PRIVATE nlohmann_json::nlohmann_json)

# copy sql scripts

# file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts)
# file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client1_db)
# file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client2_db)
# file(MAKE_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client3_db)

# file(COPY_FILE
#     ${PROJECT_SOURCE_DIR}/sql_scripts/sql_script_1_configure_db.sql
#     ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client1_db/sql_script_1_configure_db.sql)

# file(COPY_FILE
#     ${PROJECT_SOURCE_DIR}/sql_scripts/sql_script_1_configure_db.sql
#     ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client2_db/sql_script_1_configure_db.sql)

# file(COPY_FILE
#     ${PROJECT_SOURCE_DIR}/sql_scripts/sql_script_1_configure_db.sql
#     ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client3_db/sql_script_1_configure_db.sql)

# file(COPY_FILE
#     ${PROJECT_SOURCE_DIR}/sql_scripts/client1_db/sql_script_2_create_data_client1_db.sql
#     ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client1_db/sql_script_2_create_data_client1_db.sql)

# file(COPY_FILE
#     ${PROJECT_SOURCE_DIR}/sql_scripts/client2_db/sql_script_2_create_data_client2_db.sql
#     ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client2_db/sql_script_2_create_data_client2_db.sql)

# file(COPY_FILE
#     ${PROJECT_SOURCE_DIR}/sql_scripts/client3_db/sql_script_2_create_data_client3_db.sql
#     ${CMAKE_CURRENT_BINARY_DIR}/sql_scripts/client3_db/sql_script_2_create_data_client3_db.sql)
