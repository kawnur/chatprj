file(GLOB_RECURSE HEADER_LIST ${PROJECT_SOURCE_DIR}/include/*.hpp)
file(GLOB_RECURSE SOURCE_LIST ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(chatapp_lib ${HEADER_LIST} ${SOURCE_LIST})

target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/action)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/app)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/constants)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/data)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/db)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/log)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/manager)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/network)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/task)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/utils)
target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include/widgets)

target_link_libraries(chatapp_lib crypto ssl)
