
# TTMS
----
#### 介绍
这是一个在Linux系统下,开发的以C/S架构为基础的TTMS影院管理系统,后端使用ssxrver网络库,数据传输协议类似与HTTP协议,前后端数据交互使用json格式
ssxrver 仓库地址: https://github.com/RandyLambert/ssxrver
前端使用c++ qt实现 仓库地址: https://github.com/HhTtLllL/TTMS_network

#### 开发环境
1. 操作系统 : deepin 15.11桌面版
2. gcc版本 : 7.3
3. 语言 : c++ 17
4. cmake版本 : 3.11.2
6. boost库版本 : 1.72
5. 数据库版本 : MySQL 5.7.21-1

#### 部署环境
1. 确保你的cmake版本在3.11.2之上,或者修改CMakeLists.txt最低版本为你的环境cmake版本,但是会有风险编译失败
2. 确保环境内boost库版本在1.69以上,并且gcc版本7.3之上
3. 确保Linux内核版本在2.6.28以上,glibc版本在2.10以上
4. 确保在运行前系统已经安装MySQL5.7.21-1,并已经使用ttms.sql文件已经完成了对数据库创建,并完成了对数据库的配置
5. 在本目录下执行./build.sh,编译成功会生成一个build的目录,按照你所选的编译版本相应会生成Debug目录或者Release目录,执行生成的可执行文件即可运行

#### 说明

接口说明网址: https://www.showdoc.cc/TTMS?page_id=4550160801238142
使用C语言实现单机版TTMS代码地址: https://github.com/HhTtLllL/TTMS
 
