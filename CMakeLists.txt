cmake_minimum_required(VERSION 3.10)
project(sniffer C)

# 设置 C 标准
set(CMAKE_C_STANDARD 99)

# 查找 pcap 库
find_package(PkgConfig)
pkg_check_modules(PCAP QUIET libpcap)



file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/*.c
)

# 添加可执行文件
add_executable(sniffer ${SRC_FILES})

# 包含头文件目录
target_include_directories( sniffer PRIVATE
    # ${PCAP_INCLUDE_DIRS}
    ${PROJECT_SOURCE_DIR}/include
    ${CMAKE_FIND_ROOT_PATH}/usr/include
)

message(STATUS "PCAP_INCLUDE_DIRS path: ${PCAP_INCLUDE_DIRS}")
message(STATUS "Include path: ${PROJECT_SOURCE_DIR}/include")
message(STATUS "SDK Include path: ${CMAKE_FIND_ROOT_PATH}/usr/include")


# 链接 pcap 库
if(PCAP_FOUND)
    target_link_libraries(sniffer ${PCAP_LIBRARIES})
else()
    target_link_directories(sniffer PRIVATE
        ${CMAKE_FIND_ROOT_PATH}/usr/lib
    )
    target_link_libraries(sniffer
        -lpcap
    )

endif()
