#ifndef __DRIVER_H
#define __DRIVER_H

#include "gpio.h"
#include "tim.h"
#include "stdio.h"

extern uint8_t IDENTIFIER;
extern uint8_t FunCode;



static int16_t basic_speed = 140;

void L_Motor_C(int16_t num);

void L_Motor_R(int16_t num);

void R_Motor_C(int16_t num);

void R_Motor_R(int16_t num);

void GoStright(void);

void Reback(void);

void TurnLeft(int16_t mul);

void TurnRight(int16_t mul);

void Stop(void);
	
void FollowTheTrack1(void);

void FollowTheTrack2(void);

void modbusControl(uint8_t control);

#endif 
