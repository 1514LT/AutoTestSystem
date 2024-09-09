cmake_minimum_required(VERSION 3.12)
project(main)
set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib)
message(${LIB})
set(CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcServer)

add_executable(
main main.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient/TCPClient.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient/Message.cpp
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcServer/TCPServer.cpp
)

target_link_libraries(main ${LIB}/libdata_shared.so ${LIB}/libjsoncpp.so pthread
)
