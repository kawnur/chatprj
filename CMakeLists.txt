cmake_minimum_required(VERSION 3.5)

project(chatprj LANGUAGES CXX)

set(CMAKE_AUTOMOC ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FindPostgreSQL)

find_package(Boost REQUIRED COMPONENTS thread)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Network Widgets)
find_package(nlohmann_json 3.2.0 REQUIRED)

link_libraries(
    gpgme Boost::thread Qt6::Core Qt6::Gui Qt6::Network Qt6::Widgets
    pq nlohmann_json::nlohmann_json)

add_subdirectory(app)
add_subdirectory(src)

# enable_testing()

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
