cmake_minimum_required(VERSION 3.10)
project(helloworld C)

# 设置 C 标准
set(CMAKE_C_STANDARD 99)

file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/*.c
)

# 添加可执行文件
add_executable(helloworld ${SRC_FILES})

