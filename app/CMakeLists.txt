add_executable(chatapp main.cpp)

target_include_directories(chatapp PRIVATE ${PROJECT_SOURCE_DIR}/include/app)

target_link_libraries(chatapp chatapp_lib)
