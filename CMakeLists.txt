cmake_minimum_required(VERSION 3.10)
project(sniffer C)

# 设置 C 标准
set(CMAKE_C_STANDARD 99)

# 查找 pcap 库
find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED libpcap)

# 包含头文件目录
include_directories(
    ${PCAP_INCLUDE_DIRS},
    ${PROJECT_SOURCE_DIR}/include
)

file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/*.c
)

# 添加可执行文件
add_executable(main ${SRC_FILES})

# 链接 pcap 库
target_link_libraries(main ${PCAP_LIBRARIES})
