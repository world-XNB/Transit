#include "driver.h"

extern int station_stop_time;
extern int stop_flag;
extern uint8_t Pdistance;
extern int distance_pos;

extern uint8_t special_car;
extern uint8_t Carid;
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
	L_Motor_C(basic_speed + 20);
	R_Motor_C(basic_speed);
}

//@brief С�����˺���
//@parm speed:�����ٶ�
void Reback(void)
{
	L_Motor_R(basic_speed);
	R_Motor_R(basic_speed);
}

//@brief С����ת����
void TurnLeft(int16_t mul) 
{
	int16_t temp;
	temp=basic_speed*mul;
	L_Motor_C(basic_speed);
	R_Motor_C(temp);
}

//@brief С����ת����
void TurnRight(int16_t mul)
{
	int16_t temp;
	temp=basic_speed*mul+20;
	L_Motor_C(temp);
	R_Motor_C(basic_speed);
}

//@brief С���ƶ�����
void Stop(void)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_RESET);
}

//@brief ��Ȧѭ������
//ѭ��������ΪС���м���������һ��Input2;   �����Input1	�Ҷ���Input3		��һ��Input4
//��⵽����ʱ��led����I/O��Ϊ�͵�ƽ
void FollowTheTrack1(void)
{
	if(station_stop_time < 7)	//��2s��֤ģ�⹫����ս�����ټ�⵽վ���źţ�station_stop_time�����Լ����ȴ���һ��վ��
    {
	    stop_flag = 0;
	    station_stop_time = 9;
		Pdistance = 0;	//ˢ�¾���һվ����
		distance_pos++;
		if(distance_pos == 6)
			distance_pos = 0;
    }
	
	if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
    {
	    Stop();
    }
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
	{
		GoStright();
	}
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		TurnLeft(2);
	
	//0x01
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
		GoStright();
	
	
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		TurnRight(2);
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
	{ 
		stop_flag = 1;	//ͣ��

		if(station_stop_time >= 8)
			Stop();
		else
		{
			GoStright();
		}
	}
}

//@brief ��Ȧѭ������
//ѭ��������ΪС���м���������һ��Input2;   �����Input1	�Ҷ���Input3		��һ��Input4
//��⵽����ʱ��led����I/O��Ϊ�͵�ƽ
void FollowTheTrack2(void)
{
	if(station_stop_time < 7)	//��2s��֤ģ�⹫����ս�����ټ�⵽վ���źţ�station_stop_time�����Լ����ȴ���һ��վ��
    {
	    stop_flag = 0;
	    station_stop_time = 9;
		Pdistance = 0;	//ˢ�¾���һվ����
		distance_pos++;
		if(distance_pos == 4)
			distance_pos = 0;
    }
	
	if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
    {
	    Stop();
    }
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
	{
		GoStright();
	}
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		TurnLeft(2);
	
	//0x02
//	��⵽��һ����Ҷ�
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
	{
		GoStright();
	}
//	ֻ��⵽��һ
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
	{
		TurnRight(2);
	}
//	��⵽��һ����һ
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
	{
		TurnRight(2);
	}
//	��⵽����Ҷ���һ
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
	{
		TurnRight(2);
	}
		
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		TurnRight(2);
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
	{ 
		stop_flag = 1;	//ͣ��

		if(station_stop_time >= 8)
			Stop();
		else
		{
			GoStright();
		}
	}
}



//@brief ����˿���С����������
//@parm control:С�������ź�
//		0x18:one��ǰ��	0x14:one������	0x12:one����ת	0x11:one����ת
//		0x28:two��ǰ��	0x24:two������	0x22:two����ת	0x21:two����ת
//void modbusControl(uint8_t control)
//{
//	uint8_t i;
//	if(control == 0x00)		//��������
//	{
//		FunCode = 0x00;  
//		Stop();
//		return;
//	}
//	
//	i = control&0xf0;
//	printf("С����ţ�%x\n",i);
//	control = control&0x0f;
//	if(IDENTIFIER == i)
//	{
//		switch(control)
//		{
//			case 0x08: GoStright(); break;
//			case 0x04: Reback(); break;
//			case 0x02: TurnLeft(5); break;
//			case 0x01: TurnRight(5); break;
//			default: break;
//		}
//	}
//}



