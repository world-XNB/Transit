#ifndef __DRIVER_H
#define __DRIVER_H

#include "gpio.h"
#include "tim.h"

static int16_t basic_speed=150;

void L_Motor_C(int16_t num);

void L_Motor_R(int16_t num);

void R_Motor_C(int16_t num);

void R_Motor_R(int16_t num);

void GoStright(void);

void TurnLeft(int16_t mul);

void TurnRight(int16_t mul);

void Stop(void);
	
void FollowTheTrack(void);

#endif 
