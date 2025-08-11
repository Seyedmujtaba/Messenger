cmake_minimum_required(VERSION 3.10)
project(untitled2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8") # افزودن پشتیبانی از یونیکد

add_executable(untitled2
        app/main.cpp
        src/message.cpp
)

target_include_directories(untitled2 PRIVATE include)
