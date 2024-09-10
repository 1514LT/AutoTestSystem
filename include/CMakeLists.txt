cmake_minimum_required(VERSION 3.12)
project(data_library)
set(CMAKE_CXX_STANDARD 11)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

if(DEFINED ARM64)
message(STATUS "ARM64 is defined")
set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/../lib_arm64)

elseif(DEFINED X86)
message(STATUS "X86 is defined")
set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/../lib_x86)

else()
message(STATUS "X86 is defined")
set(LIB ${CMAKE_CURRENT_SOURCE_DIR}/../lib_x86)

endif()


# 添加动态库
add_library(data_shared SHARED
    ../src/dataProcess.cpp
)

# 添加静态库
add_library(data_static STATIC
    ../src/dataProcess.cpp
)

# 设置动态库和静态库的输出目录
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB})

# 链接动态库的JSONCPP依赖
target_link_libraries(data_shared PRIVATE ${LIB}/libjsoncpp.so)

# 安装动态库和静态库
install(TARGETS data_shared data_static
        LIBRARY DESTINATION ${LIB}
        ARCHIVE DESTINATION ${LIB}
)

# 安装配置文件
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/source/
        DESTINATION /root/AutoTestSystem/source/)


