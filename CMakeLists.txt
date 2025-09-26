cmake_minimum_required(VERSION 3.10)
project(KeyValueDB CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    db
    src/main.cpp 
    src/server/server.cpp
    src/db/db.cpp
    src/connection/connection.cpp
    src/utils/utils.cpp
    src/utils/response_codes.cpp
)

target_include_directories(db PRIVATE src/)