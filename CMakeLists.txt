cmake_minimum_required(VERSION 3.2)

project(hl_server C CXX)

SET (user_service "user")
SET (chat_service "chat")
SET (wall_service "wall")

find_package(OpenSSL)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)


if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
link_directories("/usr/local/lib")

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(${user_service} main_user.cpp
                                 config/config.cpp
        database/database.h database/database.cpp helper.h user_server/http_web_server.h user_server/http_request_factory.h user_server/handlers/user_handler.h database/user.h config/config.h database/user.cpp)


target_include_directories(${user_service} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${user_service} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${user_service} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${user_service} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${user_service} PRIVATE
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "PocoData"
                             "PocoDataMySQL"
                             "mysqlclient"
                             ZLIB::ZLIB)

add_executable(${chat_service} main_chat.cpp
        config/config.cpp
        database/database.h database/database.cpp helper.h database/chat.h config/config.h chat_server/handlers/chat_handler.h chat_server/http_request_factory.h chat_server/http_web_server.h database/chat.cpp)


target_include_directories(${chat_service} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${chat_service} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${chat_service} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${chat_service} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${chat_service} PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${Poco_LIBRARIES}
        "PocoData"
        "PocoDataMySQL"
        "mysqlclient"
        ZLIB::ZLIB)


add_executable(${wall_service} main_wall.cpp
        config/config.cpp
        database/database.h database/database.cpp wall_server/http_web_server.h helper.h wall_server/http_request_factory.h wall_server/handlers/wall_handler.h database/wall.h config/config.h database/wall.cpp)


target_include_directories(${wall_service} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${wall_service} PRIVATE -Wall -Wextra -pedantic -Werror )
set_target_properties(${wall_service} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${wall_service} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

target_link_libraries(${wall_service} PRIVATE
        ${CMAKE_THREAD_LIBS_INIT}
        ${Poco_LIBRARIES}
        "PocoData"
        "PocoDataMySQL"
        "mysqlclient"
        ZLIB::ZLIB)







