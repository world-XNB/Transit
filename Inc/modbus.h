#ifndef __MODBUS_H
#define __MODBUS_H


#define data_lenth 1024

// int 四个字节
// short 两个字节
// char 一个字节

//MBAP报文头
struct MBAPHead
{
    unsigned char TMIdent[2];    //Transaction meta identifier(事务元标识符)
    unsigned char PIdent[2];    //Protocol identifier(协议标识符)
    unsigned char lenth[2];    //lenth(以下字节长度)
    unsigned char UIdent;    //Unit identifier(单元标识符)——在TCP/IP中，MODBUS 单元标识符是无用的。必需使用值 0xFF
};


// MODBUS 的三种PDU
struct mb_req_pdu   // MODBUS 请求 PDU
{
    unsigned char function_code;
    unsigned char *request_data;    //具体数据长度使用时申请地址
};

struct mb_rsp_pdu   // MODBUS 响应 PDU
{
    unsigned char function_code;
    unsigned char *response_data;
};

struct mb_excep_req_pdu    // MODBUS 异常响应 PDU
{
    unsigned char function_code;  //MODBUS 功能码 + 0x80
    unsigned char exception_code;
};

// MODBUS ADU
struct MDBOS_ADU
{
    unsigned char fun_code;
    unsigned char *data;
	struct MBAPHead head;
};
//用户自定义功能码(范围: 65 至 72 和十进制 100 至 110)


struct MDBOS_ADU modbus_rsp_ADU_stat(unsigned char *rdata);
#endif
