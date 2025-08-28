cmake_minimum_required(VERSION 3.10)
project(untitled2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -finput-charset=UTF-8")


find_library(SQLITE3_LIB sqlite3)
if(SQLITE3_LIB)
    message(STATUS "Found system SQLite3: ${SQLITE3_LIB}")
else()
   
    include(FetchContent)
    FetchContent_Declare(
            sqlite3
            URL https://www.sqlite.org/2024/sqlite-amalgamation-3450200.zip
    )
    FetchContent_MakeAvailable(sqlite3)

    
    add_library(sqlite3 STATIC ${sqlite3_SOURCE_DIR}/sqlite3.c
            include/Database.h)
    target_include_directories(sqlite3 PUBLIC ${sqlite3_SOURCE_DIR})
    set(SQLITE3_LIB sqlite3)
endif()

add_executable(untitled2
        app/main.cpp
        src/MessageHandler.cpp
        src/Database.cpp
        src/Database.cpp
)

target_include_directories(untitled2 PRIVATE include ${sqlite3_SOURCE_DIR})
target_link_libraries(untitled2 PRIVATE ${SQLITE3_LIB})
