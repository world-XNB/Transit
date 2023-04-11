# This is the network part

## 项目文件介绍

1. Client 客户端项目文件夹
    + client.c 客户端代码
    + modbus.c MODBUS通信应用层代码
        > 功能码(用户自定义功能码——范围: 65 至 72 和十进制 100 至 110)
        >
        > + 0x43
        >
    + modbus.h MOBUS协议头文件
    + Makefile 构建文件

2. Server 服务器项目文件夹
    + server.c 服务器代码
    + modbus.c MODBUS通信应用层代码
        > 功能码(用户自定义功能码——范围: 65 至 72 和十进制 100 至 110)
        >
        > + 0x41(65)——服务器—>小车 请求小车经过上一站的名称，当前车速，距上一站的距离
        > + 0x42(66)——服务器—>小车 发送对小车的控制信息
        >
    + modbus.h MOBUS协议头文件
    + Makefile 构建文件
  
3. Test 测试文件夹

    > 该文件夹为第一次测试多客户端连接服务器进行通信的测试文件夹  
    + client.c 客户端代码
    + server.c 服务器代码
  
## 功能介绍

> 该模块需要实现多个客户端同时连接服务器进行相互通信的基础上实现MODBUS TCP协议  
