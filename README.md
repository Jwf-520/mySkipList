# KV存储引擎
非关系型数据库redis其核心的存储引擎是跳表。
本项目是基于跳表实现的轻量级键型存储引擎，使用C++实现。
实现的功能有：
+ 插入数据(insertElement)
+ 删除数据(deleteElement)
+ 查询数据(searchElement)
+ 展示已经存储的数据(displayList)
+ 数据落盘(dumpFile)
+ 加载数据(loadFile)
+ 返回数据(size)
在随机写读情况下，该项目每秒可处理啊请求数（QPS）: 24.39w，每秒可处理读请求数（QPS）: 18.41w。
# 项目中的文件说明
+ include文件包含头文件skiplist.h，用于跳表的核心实现
+ src文件中包含main.cpp，使用跳表进行数据操作。
+ README.md项目说明
+ bin文件中包含可执行文件的目录
+ CMakeLists.txt编译脚本
+ store数据落盘的文件存放在这个文件夹中
+ stress_test_start.sh 压力测试脚本
# 存储引擎数据表现
...
# 项目运行方式
...




