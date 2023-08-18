# Project's name
PROJECT ( example C CXX )

# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.9)

# Make sure CMake understands how we want to compile
add_definitions(-std=c++17 -Wall -Wextra)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(THREADS_PREFER_PTHREAD_FLAG ON)

# Find libraries not created by us if not throw an error so we can fix
find_package(Threads REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(CURL REQUIRED)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}/include"
                    "${PROJECT_SOURCE_DIR}/navXTimeSync")
# The following folders will be included and built by CMake 
add_subdirectory(raspberry-sbus)
add_subdirectory(Read_Json)
add_subdirectory(navXTimeSync)

# Add our file for building
add_executable(example 
    ${PROJECT_SOURCE_DIR}/Radio_Drive.cpp # Uncomment this to use SBUS
    #${PROJECT_SOURCE_DIR}/Controller_Drive.cpp # Uncomment this to use Xbox Controller
)

if (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "arm-linux-gnueabihf") # Jetson TK1 / Pi
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm32)
elseif (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "aarch64-linux-gnu") # Jetson TX2
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm64)
else()
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/x86-64)
endif()

# Add all of our libraries so that we don't get any errors
target_link_libraries(example ${PHOENIX_LIBS_DIR}/libCTRE_Phoenix.so)
target_link_libraries(example ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixCCI.so)
target_link_libraries(example ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixTools.so)
target_link_libraries(example curl)
target_link_libraries(example jsoncpp)
target_link_libraries(example Threads::Threads)
target_link_libraries(example libsbus)
target_link_libraries(example Read_Json)
target_link_libraries(example navXTimeSync)
target_link_libraries(example ${CMAKE_THREAD_LIBS_INIT})

add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PHOENIX_LIBS_DIR}
        $<TARGET_FILE_DIR:example>)