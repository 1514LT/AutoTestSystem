cmake_minimum_required(VERSION 3.12)
project(main)

if(DEFINED ARM64)
message(STATUS "ARM64 is defined")
set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib_arm64)

elseif(DEFINED X86)
message(STATUS "X86 is defined")
set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib_x86)

else()
message(STATUS "X86 is defined")
set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/lib_x86)

endif()

message(${LIB})
set(CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcServer)

add_executable(
main main.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient/TCPClient.cpp 
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient/Message.cpp
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient/FTPClient.cpp
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcServer/TCPServer.cpp
${CMAKE_CURRENT_SOURCE_DIR}/TCP/jrlcClient/Application.cpp
)

target_link_libraries(main ${LIB}/libdata_shared.so ${LIB}/libjsoncpp.so pthread
)
