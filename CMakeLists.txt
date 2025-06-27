cmake_minimum_required(VERSION 3.10)
project(openwrt-sniffer C)

# 设置 C 标准
set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -pthread")

# 查找 pcap 库
find_package(PkgConfig)
pkg_check_modules(PCAP QUIET libpcap)

# 查找pthread
find_package(Threads REQUIRED)

file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/*.c
)

# 添加可执行文件
add_executable(openwrt-sniffer ${SRC_FILES})

# 包含头文件目录
target_include_directories( openwrt-sniffer PRIVATE
    # ${PCAP_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_FIND_ROOT_PATH}/usr/include
)

message(STATUS "PCAP_INCLUDE_DIRS path: ${PCAP_INCLUDE_DIRS}")
message(STATUS "Include path: ${PROJECT_SOURCE_DIR}/include")
message(STATUS "SDK Include path: ${CMAKE_FIND_ROOT_PATH}/usr/include")


# 链接 pcap 库
if(PCAP_FOUND)
    target_link_libraries(openwrt-sniffer 
        ${PCAP_LIBRARIES}
        Threads::Threads
    )
else()
    target_link_directories(openwrt-sniffer PRIVATE
        ${CMAKE_FIND_ROOT_PATH}/usr/lib
    )
    target_link_libraries(openwrt-sniffer PRIVATE pcap)
endif()

target_link_libraries(openwrt-sniffer PRIVATE pthread)

