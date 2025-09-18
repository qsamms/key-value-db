cmake_minimum_required(VERSION 3.10)

project(key-value-db CXX)

add_executable(
    server 
    main.cpp 
    server.cpp
)