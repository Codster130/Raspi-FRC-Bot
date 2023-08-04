set(Read_Json include/Read_JSON.h include/Read_JSON.cpp)
add_library(
    Read_Json STATIC
    ${Read_Json}
)
target_include_directories(Read_Json PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")