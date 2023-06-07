#include "driver.h"

extern int station_stop_time;
extern int stop_flag;
extern uint8_t Pdistance;
extern int distance_pos;

extern uint8_t special_car;
extern uint8_t Carid;
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
	L_Motor_C(basic_speed + 20);
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
	temp=basic_speed*mul+20;
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

//@brief 外圈循迹函数
//循迹传感器为小车中间两个，左一：Input2;   左二：Input1	右二：Input3		右一：Input4
//检测到黑线时：led亮起、I/O口为低电平
void FollowTheTrack1(void)
{
	if(station_stop_time < 7)	//留2s保证模拟公交出战，不再检测到站点信号，station_stop_time不再自减，等待下一个站点
    {
	    stop_flag = 0;
	    station_stop_time = 9;
		Pdistance = 0;	//刷新距上一站距离
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
		stop_flag = 1;	//停靠

		if(station_stop_time >= 8)
			Stop();
		else
		{
			GoStright();
		}
	}
}

//@brief 内圈循迹函数
//循迹传感器为小车中间两个，左一：Input2;   左二：Input1	右二：Input3		右一：Input4
//检测到黑线时：led亮起、I/O口为低电平
void FollowTheTrack2(void)
{
	if(station_stop_time < 7)	//留2s保证模拟公交出战，不再检测到站点信号，station_stop_time不再自减，等待下一个站点
    {
	    stop_flag = 0;
	    station_stop_time = 9;
		Pdistance = 0;	//刷新距上一站距离
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
//	检测到左一左二右二
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
	{
		GoStright();
	}
//	只检测到右一
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
	{
		TurnRight(2);
	}
//	检测到左一、右一
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
	{
		TurnRight(2);
	}
//	检测到左二右二右一
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
	{
		TurnRight(2);
	}
		
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_SET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_SET)
		TurnRight(2);
	else if( HAL_GPIO_ReadPin(Input1_GPIO_Port,Input1_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input2_GPIO_Port,Input2_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input3_GPIO_Port,Input3_Pin)==GPIO_PIN_RESET&&HAL_GPIO_ReadPin(Input4_GPIO_Port,Input4_Pin)==GPIO_PIN_RESET)
	{ 
		stop_flag = 1;	//停靠

		if(station_stop_time >= 8)
			Stop();
		else
		{
			GoStright();
		}
	}
}



//@brief 服务端控制小车驱动函数
//@parm control:小车控制信号
//		0x18:one车前进	0x14:one车后退	0x12:one车左转	0x11:one车右转
//		0x28:two车前进	0x24:two车后退	0x22:two车左转	0x21:two车右转
//void modbusControl(uint8_t control)
//{
//	uint8_t i;
//	if(control == 0x00)		//结束控制
//	{
//		FunCode = 0x00;  
//		Stop();
//		return;
//	}
//	
//	i = control&0xf0;
//	printf("小车编号：%x\n",i);
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



