cmake_minimum_required(VERSION 3.12)
project(server)
set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/../../lib)
message(${LIB})
set(CMAKE_CXX_STANDARD 11)


add_executable(
server TCPServer.cpp
)

target_link_libraries(server ${LIB}/libdata_shared.so ${LIB}/libjsoncpp.so pthread
)
