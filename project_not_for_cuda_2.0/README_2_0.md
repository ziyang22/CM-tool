# 版本内容
这个版本实现了
1. 记录baseline和optimized_code的优化时间
2. 计算相对优化速率
3. 判断optimized_code结果是否正确

# 版本缺陷
1. 只能对比cpp与cpp文件不能对比cuda文件
2. 项目目录结构不完善，baseline,datebase,optimized_code等文件放置一处，稍显混乱