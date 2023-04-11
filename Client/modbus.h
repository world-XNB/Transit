#ifndef _MODBUS_H_
#define _MODBUS_H_
// #include <stdio.h>

#define data_lenth 1024

// int 四个字节
// short 两个字节
// char 一个字节

//MBAP报文头
struct MBAPHead
{
    short TMIdent;    //Transaction meta identifier(事务元标识符)
    short PIdent;    //Protocol identifier(协议标识符)
    short lenth;    //lenth(以下字节长度)
    char UIdent;    //Unit identifier(单元标识符)——在TCP/IP中，MODBUS 单元标识符是无用的。必需使用值 0xFF
};


// MODBUS 的三种PDU
struct mb_req_pdu   // MODBUS 请求 PDU
{
    char function_code;
    char request_data[data_lenth];
};

struct mb_rsp_pdu   // MODBUS 响应 PDU
{
    char function_code;
    char response_data[data_lenth];
};

struct mb_excep_req_pdu    // MODBUS 异常响应 PDU
{
    char function_code;  //MODBUS 功能码 + 0x80
    char exception_code;
};


//用户自定义功能吗(范围: 65 至 72 和十进制 100 至 110)

#endif