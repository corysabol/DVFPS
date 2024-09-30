cmake_minimum_required(VERSION 3.10)
project(DVFPSServer C)

# Specify the C standard
set(CMAKE_C_STANDARD 99)

# Export compile commands
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Find and link libcurl
find_package(CURL REQUIRED)
include_directories(${CURL_INCLUDE_DIR})

# Add source files
set(SOURCES
    src/main.c
    src/networking.c
    src/auth.c
    src/game_logic.c
)

# Include header files directory
include_directories(include)

# Define the executable to be built
add_executable(game_server ${SOURCES})

# Link libcurl
target_link_libraries(game_server ${CURL_LIBRARIES})