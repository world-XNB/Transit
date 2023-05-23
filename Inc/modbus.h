#ifndef __MODBUS_H
#define __MODBUS_H


#define data_lenth 1024

// int �ĸ��ֽ�
// short �����ֽ�
// char һ���ֽ�

//MBAP����ͷ
struct MBAPHead
{
    unsigned char TMIdent[2];    //Transaction meta identifier(����Ԫ��ʶ��)
    unsigned char PIdent[2];    //Protocol identifier(Э���ʶ��)
    unsigned char lenth[2];    //lenth(�����ֽڳ���)
    unsigned char UIdent;    //Unit identifier(��Ԫ��ʶ��)������TCP/IP�У�MODBUS ��Ԫ��ʶ�������õġ�����ʹ��ֵ 0xFF
};


// MODBUS ������PDU
struct mb_req_pdu   // MODBUS ���� PDU
{
    unsigned char function_code;
    unsigned char *request_data;    //�������ݳ���ʹ��ʱ�����ַ
};

struct mb_rsp_pdu   // MODBUS ��Ӧ PDU
{
    unsigned char function_code;
    unsigned char *response_data;
};

struct mb_excep_req_pdu    // MODBUS �쳣��Ӧ PDU
{
    unsigned char function_code;  //MODBUS ������ + 0x80
    unsigned char exception_code;
};

// MODBUS ADU
struct MDBOS_ADU
{
    unsigned char fun_code;
    unsigned char *data;
	struct MBAPHead head;
};
//�û��Զ��幦����(��Χ: 65 �� 72 ��ʮ���� 100 �� 110)


struct MDBOS_ADU modbus_rsp_ADU_stat(unsigned char *rdata);
#endif
