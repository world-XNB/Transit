#ifndef __DRIVER_H
#define __DRIVER_H

#include "gpio.h"
#include "tim.h"

void L_Motor_C(int16_t num);

void L_Motor_R(int16_t num);

void R_Motor_C(int16_t num);

void R_Motor_R(int16_t num);
	
void FollowTheTrack(void);

void GoStright(int16_t speed);

void TurnLeft(void);

void TurnRight(void);

#endif 
