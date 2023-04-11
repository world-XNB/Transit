#include "modbus.h"

// modbus 请求ADU——请求小车此时的状态
struct MDBOS_ADU modbus_req_ADU_stat()
{
    struct MDBOS_ADU ADU;
    struct MBAPHead mbap;
    struct mb_req_pdu pdu;
    char rdata = 0;
   
    
    // 初始化modbus报文头
    mbap.TMIdent = 0x1501;
    mbap.PIdent = 0x0000;
    mbap.lenth = 0x0000;
    mbap.UIdent = 0xff;

    pdu.function_code = 0x41;
    pdu.request_data = &rdata;

    ADU.head = mbap;
    ADU.fun_code = pdu.function_code;
    ADU.data = pdu.request_data;
    return ADU;
}

// modbus 请求ADU——对小车发出控制信号
struct MDBOS_ADU modbus_req_ADU_control()
{
    struct MDBOS_ADU ADU;
    struct MBAPHead mbap;
    struct mb_req_pdu pdu;
    char rdata = 0;
   
    
    // 初始化modbus报文头
    mbap.TMIdent = 0x1501;
    mbap.PIdent = 0x0000;
    mbap.lenth = 0x0000;
    mbap.UIdent = 255;

    pdu.function_code = 0x41;
    pdu.request_data = &rdata;

    ADU.head = mbap;
    ADU.fun_code = pdu.function_code;
    ADU.data = pdu.request_data;
    return ADU;
}