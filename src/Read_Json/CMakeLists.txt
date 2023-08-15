#set(Read_Json Read_JSON.h Read_JSON.cpp)
add_library(
    Read_Json
    Read_JSON.cpp
    Read_JSON.h
)
target_link_libraries(Read_Json curl)
target_link_libraries(Read_Json jsoncpp)
#target_include_directories(Read_Json PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")