# Project's name
cmake_minimum_required(VERSION 3.1)
PROJECT ( example C CXX )

# Specify the minimum version for CMake

#make_minimum_required(VERSION 3.1)

add_definitions(-std=c++11)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/pigpio/cmake)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/pigpio/cmake)

find_package (Threads REQUIRED)
#find_package(SDL2 REQUIRED)
find_package(pigpio REQUIRED)

# The following folder will be included
include_directories("${PROJECT_SOURCE_DIR}/include")

#add_executable(ctre_example ${PROJECT_SOURCE_DIR}/example.cpp)
add_executable(example ${PROJECT_SOURCE_DIR}/radio_test.cpp)

# if (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "arm-linux-gnueabihf") # Jetson TK1 / Pi
#     set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/raspberry)
# elseif (${CMAKE_LIBRARY_ARCHITECTURE} STREQUAL "aarch64-linux-gnu") # Jetson TX2
#     set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/jetsontx)
# else()
#     set(PHOENIX_LIBS_DIR ${CMAKE_SOURCE_DIR}/lib/x86-64)
# endif()

# target_link_libraries(example ${PHOENIX_LIBS_DIR}/libCTRE_Phoenix.so)
# target_link_libraries(example ${PHOENIX_LIBS_DIR}/libCTRE_PhoenixCCI.so)

# target_link_libraries(example Threads::Threads)
# target_link_libraries(example ${SDL2_LIBRARIES})

add_custom_command(TARGET example POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PHOENIX_LIBS_DIR}
        $<TARGET_FILE_DIR:example>)
