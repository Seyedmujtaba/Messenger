cmake_minimum_required(VERSION 3.10)
project(UserManagementSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(UserManagementSystem
    main.cpp
    UserManager.cpp
    Database.cpp
)
