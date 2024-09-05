cmake_minimum_required(VERSION 3.12)
project(client)
set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib)
message(${LIB})
set(CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient)

add_executable(
client client.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient/TCPClient.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient/Message.cpp
)

target_link_libraries(client ${LIB}/libdata_shared.so ${LIB}/libjsoncpp.so
)
