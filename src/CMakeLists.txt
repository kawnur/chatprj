file(GLOB HEADER_LIST ${PROJECT_SOURCE_DIR}/include/*.hpp)
file(GLOB SOURCE_LIST ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_library(chatapp_lib ${HEADER_LIST} ${SOURCE_LIST})

target_include_directories(chatapp_lib PRIVATE ${PROJECT_SOURCE_DIR}/include)
