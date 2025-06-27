set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(OPENWRT_SDK "/home/wangyanwen.wyw/working/NLab/cross-compile/openwrt-sdk-24.10.0-x86-64_gcc-13.3.0_musl.Linux-x86_64")
set(STAGING_DIR /home/wangyanwen.wyw/working/NLab/cross-compile/openwrt-sdk-24.10.0-x86-64_gcc-13.3.0_musl.Linux-x86_64/staging_dir)

# 设置交叉编译器
set(CMAKE_C_COMPILER ${STAGING_DIR}/toolchain-x86_64_gcc-13.3.0_musl/bin/x86_64-openwrt-linux-gcc)
set(CMAKE_CXX_COMPILER ${STAGING_DIR}/toolchain-x86_64_gcc-13.3.0_musl/bin/x86_64-openwrt-linux-g++)

# 设置sysroot
set(CMAKE_FIND_ROOT_PATH ${STAGING_DIR}/target-x86_64_musl)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)