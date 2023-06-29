# Project's name
PROJECT ( example C CXX )

# Specify the minimum version for CMake

cmake_minimum_required(VERSION 3.9)

add_definitions(-std=c++17)

# set(CMAKE_C_STANDARD 99)
# set(CMAKE_CXX_STANDARD 11)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package (Threads REQUIRED)
#find_package(SDL2 REQUIRED)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}/include")
add_subdirectory("${PROJECT_SOURCE_DIR}/raspberry-sbus")

add_executable(example ${PROJECT_SOURCE_DIR}/Radio_Drive.cpp)
#add_executable(example2 ${PROJECT_SOURCE_DIR}/example2.cpp)


if (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "arm-linux-gnueabihf") # Jetson TK1 / Pi
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm32)
elseif (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "aarch64-linux-gnu") # Jetson TX2
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/arm64)
else()
    set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/x86-64)
endif()

target_link_libraries(example ${PHOENIX_LIBS_DIR}/libCTRE_Phoenix.so)
target_link_libraries(example ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixCCI.so)
target_link_libraries(example ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixTools.so)

target_link_libraries(example Threads::Threads)
#target_link_libraries(example ${SDL2_LIBRARIES})
target_link_libraries(example ${PROJECT_SOURCE_DIR}/raspberry-sbus libsbus)

add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PHOENIX_LIBS_DIR}
        $<TARGET_FILE_DIR:example>)
