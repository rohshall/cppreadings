cmake_minimum_required(VERSION 2.8)
project(cppreadings)

add_executable(cppreadings server.cpp)
find_library(FCGI_LIB fcgi)
find_library(FCGI++_LIB fcgi++)
message("FCGI libs: ${FCGI_LIB} AND ${FCGI++_LIB}")
target_link_libraries(cppreadings ${FCGI_LIB} ${FCGI++_LIB})
