# 制定只用的cmake的最低版本
cmake_minimum_required(VERSION 3.0)
# 定义工程名称
project(CALC)

include_directories(${project_not_for_cuda}/Head)
# 定义工程会生成可执行程序
# add_executable(可执行程序名 源文件名称)
add_executable(app Baseline.cpp datebase.cpp examination.cpp main.cpp optimized_code.cpp)