#include "modbus.h"

// modbus 请求ADU——
void modbus_req_ADU()
{
    struct MBAPHead mbap;
    mbap.TMIdent = 0x1501;
    mbap.PIdent = 0x0000;
    mbap.lenth = 0x0000;
    mbap.UIdent = 0xff;
}