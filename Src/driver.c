#include "driver.h"


//--------------------------------------------------------------------------------------------------
//该工程中的正转为小车向前时的轮子转动方向，左右两边的正转实际相反
//--------------------------------------------------------------------------------------------------


//@brief 左电机正转
//@parm num：设置占空比（自动重装载系数为1000）
void L_Motor_C(int16_t num)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,num);
}

//@brief 左电机反转
//@parm num：设置占空比（自动重装载系数为1000）
void L_Motor_R(int16_t num)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,num);
}

//@brief 右电机正转
//@parm num：设置占空比（自动重装载系数为1000）
void R_Motor_C(int16_t num)
{
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,num);
}

//@brief 右电机反转
//@parm num：设置占空比（自动重装载系数为1000）
void R_Motor_R(int16_t num)
{
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,num);
}


//@brief 小车前进函数
//@parm speed:前进速度
void GoStright(int16_t speed)
{
	L_Motor_C(speed);
	R_Motor_C(speed);
}

void TurnLeft(void) 
{
	L_Motor_C(400);
	R_Motor_C(100);
}

void TurnRight(void)
{
	L_Motor_C(100);
	R_Motor_C(400);
}

//@brief 循迹函数
//循迹传感器为小车左侧第二个，第三个
void FollowTheTrack(void)
{
  if( HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET)
		GoStright(0);
	else if( HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET)
		GoStright(100);
	else if( HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET)
		TurnRight();
	else if( HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET)
		TurnLeft();
}
