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
	temp=basic_speed*mul;
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

//@brief ѭ������
//ѭ��������ΪС���м��������Ҷ���Input2;�����Input3
//��⵽����ʱ��led����I/O��Ϊ�͵�ƽ
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
//	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&
//		HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
//		GoStright();
}

//@brief ����˿���С����������
//@parm control:С�������ź�
//		0x18:one��ǰ��	0x14:one������	0x12:one����ת	0x11:one����ת
//		0x28:two��ǰ��	0x24:two������	0x22:two����ת	0x21:two����ת
void modbusControl(uint8_t control)
{
	uint8_t i;
	if(control == 0x00)		//��������
	{
		FunCode = 0x00;  
		Stop();
		return;
	}
	
	i = control&0xf0;
	printf("С����ţ�%x\n",i);
	control = control&0x0f;
	if(IDENTIFIER == i)
	{
		switch(control)
		{
			case 0x08: GoStright(); break;
			case 0x04: Reback(); break;
			case 0x02: TurnLeft(5); break;
			case 0x01: TurnRight(5); break;
			default: break;
		}
	}
}

//uint8_t Direction;	//������ת������
//uint16_t counter;	//����������������printf�����Ķ�ʱ����
//uint16_t enc1 = 0,enc1_old = 0;		//����������
//int16_t enc2 = 0;	//enc1�Ľ�λ
//int32_t enc;	//���ձ������ļ���ֵ

//// @brief: ������
//int32_t encoder()
//{
//	Direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);  
//    enc1 = (uint32_t)(__HAL_TIM_GET_COUNTER(&htim3));	//��ȡ��ʱ����ֵ
//	if((Direction == 0) &(enc1 < enc1_old))				//������ת��ֵ��С,˵����λ
//	{
//		enc2++;
//	}
//	if((Direction == 1) &(enc1 > enc1_old))				//������ת��ֵ��С,˵����λ
//	{
//		enc2--;
//	}
//	enc1_old = enc1;									//����enc1_old�������´μ���
//	enc = enc2<<16 | enc1;								//���㵱ǰ������ֵ����+-��
//	counter++;											//����������
//	if(counter>1000)									//������ÿ������Լ1ms���˴�����ÿ1000ms����һ����
//	{
//		counter = 0;									//����ֵ����
//		printf("Dir %d,	Enc2 %d, Enc1 %d, ENC %d\r\n",Direction,enc2,enc1,enc);//��ӡ��ؼ�������
//	}
//	return enc;
//}

