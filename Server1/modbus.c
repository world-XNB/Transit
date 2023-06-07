#include "modbus.h"

// modbus 请求ADU——请求小车此时的状态
struct MDBOS_ADU modbus_req_ADU_stat()
{
    struct MDBOS_ADU ADU;
    struct MBAPHead mbap;
    struct mb_req_pdu pdu;
    unsigned char rdata[] = {};
   
    
    // 初始化modbus报文头
    mbap.TMIdent[0] = 0x15;
    mbap.TMIdent[1] = 0x01;
    mbap.PIdent[0] = 0x00;
    mbap.PIdent[1] = 0x00;
    mbap.lenth[0] = 0x02;   //ADU在该字节后面还有2字节（包含2字节的单元标识符,功能码）
    mbap.lenth[1] = 0x00;   
    mbap.UIdent = 0xff;

    pdu.function_code = 0x41;
    pdu.request_data = rdata;

    ADU.head = mbap;
    ADU.fun_code = pdu.function_code;
    ADU.data = pdu.request_data;
    return ADU;
}

// modbus 请求ADU——对小车发出控制信号
// @param control:公交车编号
struct MDBOS_ADU modbus_req_ADU_control(unsigned char control)
{
    struct MDBOS_ADU ADU;
    struct MBAPHead mbap;
    struct mb_req_pdu pdu;
    
    // 初始化modbus报文头——注意：存储方式是小端存储还是大端存储
    mbap.TMIdent[0] = 0x15;
    mbap.TMIdent[1] = 0x01;
    mbap.PIdent[0] = 0x00;
    mbap.PIdent[1] = 0x00;
    mbap.lenth[0] = 0x03;   //lenth[0]是低位数据
    mbap.lenth[1] = 0x00;
    mbap.UIdent = 0xff;

    pdu.function_code = 0x45;
    pdu.request_data = &control;

    ADU.head = mbap;
    ADU.fun_code = pdu.function_code;
    ADU.data = pdu.request_data;
    return ADU;
}

// modbus 响应ADU  服务器—>客户端：返回站点信息
struct MDBOS_ADU modbus_res_ADU_station(unsigned char *info)
{

    struct MDBOS_ADU ADU;
    struct MBAPHead mbap;
    struct mb_req_pdu pdu;
    
    // 初始化modbus报文头——注意：存储方式是小端存储还是大端存储
    mbap.TMIdent[0] = 0x15;
    mbap.TMIdent[1] = 0x01;
    mbap.PIdent[0] = 0x00;
    mbap.PIdent[1] = 0x00;
    mbap.lenth[0] = 0x06;   //lenth[0]是低位数据
    mbap.lenth[1] = 0x00;
    mbap.UIdent = 0xff;

    pdu.function_code = 0x43;
    pdu.request_data = info;

    ADU.head = mbap;
    ADU.fun_code = pdu.function_code;
    ADU.data = pdu.request_data;
    return ADU;
}

//请求 服务器->模拟公交 模拟公交启动
struct MDBOS_ADU modbus_req_ADU_start()
{
    struct MDBOS_ADU ADU;
    struct MBAPHead mbap;
    struct mb_req_pdu pdu;
    unsigned char rdata[] = {};
    
    // 初始化modbus报文头——注意：存储方式是小端存储还是大端存储
    mbap.TMIdent[0] = 0x15;
    mbap.TMIdent[1] = 0x01;
    mbap.PIdent[0] = 0x00;
    mbap.PIdent[1] = 0x00;
    mbap.lenth[0] = 0x02;   //lenth[0]是低位数据
    mbap.lenth[1] = 0x00;
    mbap.UIdent = 0xff;

    pdu.function_code = 0x46;
    pdu.request_data = rdata;

    ADU.head = mbap;
    ADU.fun_code = pdu.function_code;
    ADU.data = pdu.request_data;
    return ADU;
}

//请求 服务器->模拟公交 模拟公交结束
struct MDBOS_ADU modbus_req_ADU_end()
{
    struct MDBOS_ADU ADU;
    struct MBAPHead mbap;
    struct mb_req_pdu pdu;
    unsigned char rdata[] = {};
    
    // 初始化modbus报文头——注意：存储方式是小端存储还是大端存储
    mbap.TMIdent[0] = 0x15;
    mbap.TMIdent[1] = 0x01;
    mbap.PIdent[0] = 0x00;
    mbap.PIdent[1] = 0x00;
    mbap.lenth[0] = 0x02;   //lenth[0]是低位数据
    mbap.lenth[1] = 0x00;
    mbap.UIdent = 0xff;

    pdu.function_code = 0x47;
    pdu.request_data = rdata;

    ADU.head = mbap;
    ADU.fun_code = pdu.function_code;
    ADU.data = pdu.request_data;
    return ADU;
}



