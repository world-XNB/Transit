#include "modbus.h"

// @brief: modbus ��ӦADU������ӦС����ʱ��״̬
// @param rdata:�洢���ݵ�ָ��
struct MDBOS_ADU modbus_rsp_ADU_stat(unsigned char *rdata)
{
    struct MDBOS_ADU ADU;
    struct MBAPHead mbap;
    struct mb_req_pdu pdu;
   
    
    // ��ʼ��modbus����ͷ
    mbap.TMIdent[0] = 0x15;
    mbap.TMIdent[1] = 0x01;
    mbap.PIdent[0] = 0x00;
    mbap.PIdent[1] = 0x00;
    mbap.lenth[0] = 0x06;   //ADU�ڸ��ֽں��滹��2�ֽڣ�����2�ֽڵĵ�Ԫ��ʶ��,������+4�ֽ�rdata��
    mbap.lenth[1] = 0x00;   
    mbap.UIdent = 0xff;

    pdu.function_code = 0x41;
    pdu.request_data = rdata;

    ADU.head = mbap;
    ADU.fun_code = pdu.function_code;
    ADU.data = pdu.request_data;
    return ADU;
}
