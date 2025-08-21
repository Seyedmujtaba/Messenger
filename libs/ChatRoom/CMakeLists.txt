cmake_minimum_required(VERSION 3.12)
project(ChatMessengerProject)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# If using Qt
find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    # Add other include directories if needed
)

# Source files - ONLY .cpp files
add_library(ChatRoomLib STATIC
    ChatRoom.cpp
    # Add other .cpp files for different modules:
    # UserManagement.cpp
    # DatabaseManager.cpp 
    # MessageService.cpp
    # BotManager.cpp
)

# If creating an executable
add_executable(ChatMessenger
    main.cpp
    # Other main source files
)

# Link libraries
target_link_libraries(ChatMessenger
    ChatRoomLib
    Qt6::Core
    Qt6::Widgets
    Qt6::Network
    # SQLite3 if needed
    # ${SQLite3_LIBRARIES}
)

# If using SQLite
# find_package(SQLite3 REQUIRED)
# target_link_libraries(ChatMessenger SQLite::SQLite3)

# Install instructions (optional)
install(TARGETS ChatMessenger DESTINATION bin)
