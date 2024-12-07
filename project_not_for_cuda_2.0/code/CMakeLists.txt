# 制定只用的cmake的最低版本
cmake_minimum_required(VERSION 3.0)
# 定义工程名称
project(CALC)

include_directories(${project_not_for_cuda_2.0}/Head)
# 定义工程会生成可执行程序
# add_executable(可执行程序名 源文件名称)
file(GLOB_RECURSE cpp_files "*.cpp")
message("Found C++ files: ${cpp_files}")

set(SRC_LIST ${cpp_files})
add_executable(app ${SRC_LIST})