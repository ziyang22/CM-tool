# 版本内容
这个版本实现了
1. 判断optimized_code结果是否正确
2. 记录baseline和optimized_code的优化时间
3. 计算相对优化速率
4. 初步完善目录结构

# 版本缺陷
1. 只能对比cpp与cpp文件不能对比cuda文件
2. 未有文件结构/代码解析和使用手册

# 复现此版本你能够学习到：
1. 项目文件夹的构建和文件的分类
2. cmake和头文件的较进阶使用【特点是你知道该如何跨文件夹引用文件】