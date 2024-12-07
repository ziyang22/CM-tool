# 使用这个脚本时，你需要在bash终端环境中【最简单的bash环境是git bash】

#!/bin/powershell
# 转到build目录下
cd build
# 指定生成器为 MinGW Makefiles
# MinGW是windos 下的一个开发环境
# 所以你使用时需要确保你已经安装了MINBGW
cmake -G "MinGW Makefiles" ../code
# 使用 mingw32-make 构建项目
mingw32-make
# 运行生成的exe文件
./app
