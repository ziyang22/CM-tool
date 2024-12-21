# 版本内容
这个版本实现了
1. 判断optimized_code结果是否正确
2. 记录baseline和optimized_code的优化时间
3. 计算相对优化速率


# 版本缺陷
1. 只能对比cpp与cpp文件不能对比cuda文件
2. 项目目录结构不完善，baseline,datebase,optimized_code等文件放置一处，稍显混乱
3. 未有文件结构/代码解析和使用手册

# 复现此版本你能够学习到：
1. 项目文件的规划
2. 头文件（.hpp .h）的书写
3. 脚本（.sh）的构建
4. 基本的cmake和make的使用