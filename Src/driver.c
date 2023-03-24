#include "driver.h"


//--------------------------------------------------------------------------------------------------
//�ù����е���תΪС����ǰʱ������ת�������������ߵ���תʵ���෴
//--------------------------------------------------------------------------------------------------


//@brief ������ת
//@parm num������ռ�ձȣ��Զ���װ��ϵ��Ϊ1000��
void L_Motor_C(int16_t num)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,num);
}

//@brief ������ת
//@parm num������ռ�ձȣ��Զ���װ��ϵ��Ϊ1000��
void L_Motor_R(int16_t num)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,num);
}

//@brief �ҵ����ת
//@parm num������ռ�ձȣ��Զ���װ��ϵ��Ϊ1000��
void R_Motor_C(int16_t num)
{
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,num);
}

//@brief �ҵ����ת
//@parm num������ռ�ձȣ��Զ���װ��ϵ��Ϊ1000��
void R_Motor_R(int16_t num)
{
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,num);
}


//@brief С��ǰ������
//@parm speed:ǰ���ٶ�
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

//@brief ѭ������
//ѭ��������ΪС�����ڶ�����������
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
