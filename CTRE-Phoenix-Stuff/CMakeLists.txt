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
#find_package(Boost REQUIRED COMPONENTS
    # system
    # program_options
    # filesystem
    # date_time
    # context
    # coroutine
    # chrono
    # log
    # )
#find_library(restc-cpp REQUIRED)
#find_package(SDL2 REQUIRED)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}/include")  
include_directories(~/restc-cpp/include/restc-cpp)
add_subdirectory("${PROJECT_SOURCE_DIR}/raspberry-sbus")
#include_directories("${PROJECT_SOURCE_DIR}/include/Read_Json") 

add_executable(example ${PROJECT_SOURCE_DIR}/Radio_Drive.cpp)
#add_executable(json ${PROJECT_SOURCE_DIR}/Read_JSON.cpp)
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
target_link_libraries(example curl)
target_link_libraries(example jsoncpp)

#target_link_libraries(example ${CMAKE_SOURCE_DIR}/include/Read_Json)
target_link_libraries(example Threads::Threads)
#target_link_libraries(example ${SDL2_LIBRARIES})
target_link_libraries(example ${PROJECT_SOURCE_DIR}/raspberry-sbus libsbus)

#target_link_libraries(json $~/restc-cpp/include/restc-cpp)

# target_include_directories(${PROJECT_NAME} PUBLIC
#     $<BUILD_INTERFACE:${ZLIB_INCLUDE_DIR}>
#     $<BUILD_INTERFACE:${ZLIB_INCLUDE_DIR}/build>
#     $<BUILD_INTERFACE:${OPENSSL_INCLUDE_DIR}>
# )

add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PHOENIX_LIBS_DIR}
        $<TARGET_FILE_DIR:example>)
