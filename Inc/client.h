#ifndef __CLIENT_H
#define __CLIENT_H

#include <stdint.h>
#include "usart.h"

void Connect_Wifi(void);

void Connect_IP(void);

void Break_IP(void);

void Send_Data(uint8_t *data ,int lenth);

#endif
