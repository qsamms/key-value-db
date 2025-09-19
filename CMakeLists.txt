cmake_minimum_required(VERSION 3.10)
project(key-value-db CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(
    server 
    main.cpp 
    server.cpp
    db.cpp
    connection_handler.cpp
    utils.cpp
    response_codes.cpp
)