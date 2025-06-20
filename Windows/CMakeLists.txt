cmake_minimum_required(VERSION 3.31)
project(Shit)

set(CMAKE_CXX_STANDARD 17)

# 定义UNICODE和_UNICODE宏
add_definitions(-DUNICODE -D_UNICODE)

# 设置为Win32应用程序，使用MinGW兼容的标志
if(MINGW)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
else()
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
endif()

# 添加资源文件
set(RESOURCES version.rc)

add_executable(Shit WIN32 main.cpp ${RESOURCES})
