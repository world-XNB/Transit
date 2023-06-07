#include "modbus.h"

// @brief: modbus 响应ADU——响应小车此时的状态
// @param rdata:存储数据的指针
struct MDBOS_ADU modbus_rsp_ADU_stat(unsigned char *rdata)
{
    struct MDBOS_ADU ADU;
    struct MBAPHead mbap;
    struct mb_req_pdu pdu;
   
    
    // 初始化modbus报文头
    mbap.TMIdent[0] = 0x15;
    mbap.TMIdent[1] = 0x01;
    mbap.PIdent[0] = 0x00;
    mbap.PIdent[1] = 0x00;
    mbap.lenth[0] = 0x06;   //ADU在该字节后面还有2字节（包含2字节的单元标识符,功能码+4字节rdata）
    mbap.lenth[1] = 0x00;   
    mbap.UIdent = 0xff;

    pdu.function_code = 0x41;
    pdu.request_data = rdata;

    ADU.head = mbap;
    ADU.fun_code = pdu.function_code;
    ADU.data = pdu.request_data;
    return ADU;
}
