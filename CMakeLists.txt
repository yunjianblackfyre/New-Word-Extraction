# CMake 最低版本号要求
cmake_minimum_required (VERSION 2.8)

# 项目信息
project (newWords)

# 设置为支持c++11
set (CMAKE_CXX_STANDARD 11)

# 指定二进制文件输出路径
set(dir ${CMAKE_CURRENT_SOURCE_DIR}/build/bin)

# 设置二进制文件输出路径
set(EXECUTABLE_OUTPUT_PATH ${dir})

# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)

# 加入头文件
include_directories(/home/caonimabi/develop/job_data_mining/cpp_new_word/include)

# 加入库文件
link_libraries("/home/caonimabi/develop/job_data_mining/cpp_new_word/lib/libboost_regex.a" "/home/caonimabi/develop/job_data_mining/cpp_new_word/lib/libjson_linux-gcc-5.4.0_libmt.a" "/home/caonimabi/develop/job_data_mining/cpp_new_word/lib/libmysqlclient.so")

add_subdirectory(/home/caonimabi/develop/job_data_mining/cpp_new_word/src)

# 指定生成目标
add_executable(newWords main.cpp)

target_link_libraries(newWords srcFiles)
