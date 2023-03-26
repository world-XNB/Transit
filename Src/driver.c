#include "driver.h"


//--------------------------------------------------------------------------------------------------
//�ù����е���תΪС����ǰʱ������ת�������������ߵ���תʵ���෴
//--------------------------------------------------------------------------------------------------


//@brief ������ת
//@parm num������ռ�ձȣ��Զ���װ��ϵ��Ϊ1000��
void L_Motor_C(int16_t num)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,num);
}

//@brief ������ת
//@parm num������ռ�ձȣ��Զ���װ��ϵ��Ϊ1000��
void L_Motor_R(int16_t num)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,num);
}

//@brief �ҵ����ת
//@parm num������ռ�ձȣ��Զ���װ��ϵ��Ϊ1000��
void R_Motor_C(int16_t num)
{
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,num);
}

//@brief �ҵ����ת
//@parm num������ռ�ձȣ��Զ���װ��ϵ��Ϊ1000��
void R_Motor_R(int16_t num)
{
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,num);
}


//@brief С��ǰ������
//@parm speed:ǰ���ٶ�
void GoStright(void)
{
	L_Motor_C(basic_speed);
	R_Motor_C(basic_speed);
}

//brief С����ת����
void TurnLeft(int16_t mul) 
{
	int16_t temp;
	temp=basic_speed*mul;
	L_Motor_C(basic_speed);
	R_Motor_C(temp);
}

//brief С����ת����
void TurnRight(int16_t mul)
{
	int16_t temp;
	temp=basic_speed*mul;
	L_Motor_C(temp);
	R_Motor_C(basic_speed);
}

//brief С���ƶ�����
void Stop(void)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_RESET);
}

//@brief ѭ������
//ѭ��������ΪС���м��������Ҷ���Input2;�����Input3
//led����I/O�ڼ�⵽�͵�ƽ
void FollowTheTrack(void)
{
  if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&
		HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		Stop();
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&
		HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		GoStright();
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&
		HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		TurnLeft(3);
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&
		HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		TurnLeft(5);
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&
		HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		TurnRight(3);
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&
		HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
		TurnRight(5);
}
