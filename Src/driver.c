#include "driver.h"


//--------------------------------------------------------------------------------------------------
//该工程中的正转为小车向前时的轮子转动方向，左右两边的正转实际相反
//--------------------------------------------------------------------------------------------------


//@brief 左电机正转
//@parm num：设置占空比（自动重装载系数为1000）
void L_Motor_C(int16_t num)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,num);
}

//@brief 左电机反转
//@parm num：设置占空比（自动重装载系数为1000）
void L_Motor_R(int16_t num)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,num);
}

//@brief 右电机正转
//@parm num：设置占空比（自动重装载系数为1000）
void R_Motor_C(int16_t num)
{
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_SET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,num);
}

//@brief 右电机反转
//@parm num：设置占空比（自动重装载系数为1000）
void R_Motor_R(int16_t num)
{
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_SET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_RESET);
	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,num);
}


//@brief 小车前进函数
//@parm speed:前进速度
void GoStright(void)
{
	L_Motor_C(basic_speed);
	R_Motor_C(basic_speed);
}

//@brief 小车后退函数
//@parm speed:后退速度
void Reback(void)
{
	L_Motor_R(basic_speed);
	R_Motor_R(basic_speed);
}

//@brief 小车左转函数
void TurnLeft(int16_t mul) 
{
	int16_t temp;
	temp=basic_speed*mul;
	L_Motor_C(basic_speed);
	R_Motor_C(temp);
}

//@brief 小车右转函数
void TurnRight(int16_t mul)
{
	int16_t temp;
	temp=basic_speed*mul;
	L_Motor_C(temp);
	R_Motor_C(basic_speed);
}

//@brief 小车制动函数
void Stop(void)
{
	HAL_GPIO_WritePin(Output1_GPIO_Port,Output1_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output2_GPIO_Port,Output2_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output3_GPIO_Port,Output3_Pin,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(Output4_GPIO_Port,Output4_Pin,GPIO_PIN_RESET);
}

//@brief 循迹函数
//循迹传感器为小车中间两个，右二：Input2;左二：Input3
//检测到黑线时：led亮起、I/O口为低电平
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

//@brief 服务端控制小车驱动函数
//@parm control:小车控制信号
//		0x18:one车前进	0x14:one车后退	0x12:one车左转	0x11:one车右转
//		0x28:two车前进	0x24:two车后退	0x22:two车左转	0x21:two车右转
void modbusControl(uint8_t control)
{
	uint8_t i;
	if(control == 0x00)		//结束控制
	{
		FunCode = 0x00;  
		Stop();
		return;
	}
	
	i = control&0xf0;
	printf("小车编号：%x\n",i);
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

//uint8_t Direction;	//编码器转动方向
//uint16_t counter;	//主函数计数，用于printf函数的定时发送
//uint16_t enc1 = 0,enc1_old = 0;		//编码器计数
//int16_t enc2 = 0;	//enc1的进位
//int32_t enc;	//最终编码器的计数值

//// @brief: 编码器
//int32_t encoder()
//{
//	Direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);  
//    enc1 = (uint32_t)(__HAL_TIM_GET_COUNTER(&htim3));	//获取定时器的值
//	if((Direction == 0) &(enc1 < enc1_old))				//正向旋转数值变小,说明进位
//	{
//		enc2++;
//	}
//	if((Direction == 1) &(enc1 > enc1_old))				//反向旋转数值变小,说明借位
//	{
//		enc2--;
//	}
//	enc1_old = enc1;									//更新enc1_old，便于下次计算
//	enc = enc2<<16 | enc1;								//计算当前计数总值，带+-号
//	counter++;											//主函数计数
//	if(counter>1000)									//主函数每次运行约1ms，此处用于每1000ms发送一次数
//	{
//		counter = 0;									//计数值清零
//		printf("Dir %d,	Enc2 %d, Enc1 %d, ENC %d\r\n",Direction,enc2,enc1,enc);//打印相关计数数据
//	}
//	return enc;
//}

