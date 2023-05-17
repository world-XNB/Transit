/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "driver.h"
#include "client.h"
#include "string.h"
#include "stdlib.h"
#include "modbus.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t IDENTIFIER = 0x10;		//小车标识符，此时为一号

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t u1_Rxch=1;

uint8_t u3_Rxlenth=0;
uint8_t u3_Rxbuff[255];
uint8_t u3_Rxch=0;

uint8_t recvflag = 0;	//usart3接受回调函数里的标志。0：代表此时接受的是AT指令标志。1：代表此时接受的是服务器数据（透传模式）

struct MDBOS_ADU ADU;

uint8_t recving_ADU_flag = 0;	//是否正在接受MODBUS_ADU。0：否；1：是。
uint8_t lenthbuff = 6;	//用来标识此时MODBUS_ADU_head+fun_code有多少字节数据,初始化为6,既此时需要接受6字节数据（2字节协议标识符、2字节后续字节长度、1字节单元标识符、1字节功能码）
uint16_t ADU_head_lenth = 0;	//用来存储MODBUS_ADU_head_lenth
uint8_t ADU_data_pos = 0;	//用来标识ADU_data的下标，数组从零开始
uint8_t data_buff[1024];
uint8_t ADU_data_lenth = 0;

uint8_t FunCode = 0x00;	//小车用来存储MODBUS功能码的中间变量
uint8_t Carspeed = 0;
uint8_t Pstation = 0;
uint8_t Pdistance = 0;
uint8_t cardata[3];	//记录公交此时状态


uint16_t last_count;
uint16_t curr_count;
uint8_t direct = 0;	//编码器正转0，反转1
uint16_t overflow = 0;	//编码器溢出标志
uint16_t count_temp = 0;
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void CarStat(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	
	HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
	last_count = __HAL_TIM_GET_COUNTER(&htim3);	//记录当前计数值
	HAL_TIM_Base_Start_IT(&htim4);
	
	HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//提前启动串口中断准备接受发送AT指令之后的返回数据
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&u1_Rxch,1);	//启动串口中断接收，必须的
	
	Connect_Wifi();		//连接局域网
	Connect_IP();	//连接IP
	recvflag = 1;	  //此时过后为透传模式下的数据传输
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);	//获取编码器方向
	  if(FunCode == 0x00)
	  {
		  FollowTheTrack();
		  if(u1_Rxch == 0x00)	//发送指令断开IP连接
		  {
			  recvflag = 0;
			  Break_IP();
			  break;
		  }
		  else
			  HAL_UART_Receive_IT(&huart1,(uint8_t *)&u1_Rxch,1);	//启动串口中断接收，必须的
	  }
	  else
	  {
		  if(u1_Rxch == 0x00)	//发送指令断开IP连接
		  {
			  recvflag = 0;
			  Break_IP();
			  break;
		  }
		  else
			  HAL_UART_Receive_IT(&huart1,(uint8_t *)&u1_Rxch,1);	//启动串口中断接收，必须的
		  
		  switch(FunCode)
		  {
//			  case 0x00: break;
			  case 0x41: printf("正在请求小车此时状态！！！\n");  CarStat(); Send_Data(cardata); FunCode = 0x00; break;
			  case 0x42: printf("正在控制小车：%x\n",ADU.data[0]); modbusControl(ADU.data[0]); FunCode = 0x00; break;
			  default: FunCode = 0x00; break;
		  }
	  }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
	
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void CarStat()
{
	cardata[0] = Carspeed;
	cardata[1] = Pstation;
	cardata[2] = Pdistance;
}

//定时器中断回调函数
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim4)
	{
		curr_count = __HAL_TIM_GET_COUNTER(&htim3);
		if(direct == 0)
		{
			count_temp = curr_count+(4000-last_count)+(overflow-1)*4000;	//编码器1s接收到的信号
			count_temp=count_temp/14;	//编码器转速（电机转一圈14个信号）
			Carspeed = count_temp/10 ;	//电机减速比10
			Pstation = Pstation + Carspeed;
			
//			printf("direct:%d\tCarspeed:%x\t%d\n",direct,Carspeed,Carspeed);
		}
		else if(direct == 1)
		{
			count_temp = last_count+(4000-curr_count)+(overflow-1)*4000;	//编码器1s接收到的信号
			count_temp=count_temp/14;	//编码器转速（电机转一圈14个信号）
			Carspeed = count_temp/10;	//电机减速比10
			Pstation = Pstation - Carspeed;
	
//			printf("direct:%d\tCarspeed:%x\t%d\n",direct,Carspeed,Carspeed);
		}
		
		overflow = 0;
		last_count = curr_count;
		count_temp = 0;
	}
	else if(htim == &htim3)
	{
		
		if(direct == 0)
			overflow++;
		else if(direct == 1)
			overflow--;
	}
}

//USART发送中断回调函数
//@brief Transit callback function for USART3
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{	
    if(huart->Instance == USART3)//判断串口号
    {
		HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//启动串口中断接收，必须的
    }
}

//USART接受中断服务函数
//@brief Rewrite callback function for USART3
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{	
    if(huart->Instance == USART3)//判断串口号——usart3不定长接受数据
    {
		u3_Rxlenth++;
		u3_Rxbuff[u3_Rxlenth-1] = u3_Rxch;
		if(recvflag == 0)
		{
			if(u3_Rxch == 0x0A&&u3_Rxbuff[u3_Rxlenth-2] == 0x0D&&u3_Rxbuff[u3_Rxlenth-3] == 0x4B&&u3_Rxbuff[u3_Rxlenth-4] == 0x4F)	//结束标志 0x4f 0x4b 0x0d 0x0a(“OK\r\n”)
			{
				printf("Recedatalenth:%d\n",u3_Rxlenth);
//	 			for(int i=0;i<u3_Rxlenth;i++)
//	 			{
//	 				printf("%x\t",u3_Rxbuff[i]);
//	 			}
//	 			printf("\n");
//				for(int i=0;i<u3_Rxlenth;i++)
//				{
//					printf("%c",u3_Rxbuff[i]);
//				}
//				printf("\n");
				printf("%s\n",u3_Rxbuff);
				memset(u3_Rxbuff,0,sizeof(u3_Rxbuff));
				u3_Rxlenth=0;
			}
			u3_Rxch = 0;
			HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//启动串口中断接收，必须的（准备接受下次数据）
		}
		else if(recvflag == 1)
		{	
			if(recving_ADU_flag == 0)	//此时接受的是普通数据
			{
				if(u3_Rxch == 0x00)	//结束标志 0x0(“\0”)
				{
					printf("%s",u3_Rxbuff);
					memset(u3_Rxbuff,0,sizeof(u3_Rxbuff));
					u3_Rxlenth=0;
				}
				else if(u3_Rxch == 0x01&&u3_Rxbuff[u3_Rxlenth-2] == 0x15)	//获取MODBUS_ADU事务处理标识符
				{
					recving_ADU_flag = 1;
					ADU.head.TMIdent[1] = u3_Rxch;
					ADU.head.TMIdent[0] = u3_Rxbuff[u3_Rxlenth-2];
					
					memset(data_buff,0,sizeof(data_buff));
				}
			}	
			else	//此时正在接受MODBUS_ADU
			{
				switch(lenthbuff)
				{
					case 0 : ADU_head_lenth = *(uint8_t *)ADU.head.lenth; break;
					case 1 : ADU.fun_code = u3_Rxch; break;
					case 2 : ADU.head.UIdent = u3_Rxch; break;
					case 3 : ADU.head.lenth[0] = u3_Rxch; break;
					case 4 : ADU.head.lenth[1] = u3_Rxch; break;
					case 5 : ADU.head.PIdent[0] = u3_Rxch; break;
					case 6 : ADU.head.PIdent[1] = u3_Rxch; break;
					default : break;
				}
			
				lenthbuff = lenthbuff - 1;	
				
				if(ADU_head_lenth != 0)
					ADU_data_lenth = ADU_head_lenth - 2;
					
				if(ADU_data_lenth > 0)	//此时开始接受ADU_data数据
				{
					data_buff[ADU_data_pos] = u3_Rxch;
					ADU_data_pos++;
					
					if(ADU_data_lenth == 1)	//数据接受即将结束
					{				
						ADU.data = data_buff;	//将MODBUS_ADU_DATA指向我们的数据缓冲区
						
						//为下一次接受做准备
						memset(u3_Rxbuff,0,sizeof(u3_Rxbuff));

						u3_Rxlenth = 0;
						recving_ADU_flag = 0;
						lenthbuff = 6;
						ADU_data_pos = 0;	
						ADU_head_lenth = 0;
					}
					
					ADU_data_lenth--;
				}
				else if(ADU_head_lenth == 2)	//请求ADU
				{			
					//为下一次接受做准备
					memset(u3_Rxbuff,0,sizeof(u3_Rxbuff));
					
					u3_Rxlenth = 0;
					recving_ADU_flag = 0;
					lenthbuff = 6;
					ADU_data_pos = 0;
					ADU_head_lenth = 0;
				}
				
				if(recving_ADU_flag == 0)	//接受完成，打印
				{
					FunCode = ADU.fun_code;	//	标注此次MODBUS功能码
					
					printf("TMIdent[0]:%x\nTMIdent[1]:%x\n",ADU.head.TMIdent[0],ADU.head.TMIdent[1]);
					printf("PIdent[0]:%x\n",ADU.head.PIdent[0]); 
					printf("PIdent[1]:%x\n",ADU.head.PIdent[1]);
					printf("lenth[0]:%x\n",ADU.head.lenth[0]); 
					printf("lenth[1]:%x\n",ADU.head.lenth[1]);
					printf("UIdent:%x\n",ADU.head.UIdent);
					printf("fun_code:%x\n",ADU.fun_code);
					
					uint16_t databu =  *(uint8_t *)ADU.head.lenth - 2;
					int p = 0;
					while(databu)
					{
						printf("ADU.data[%d]:%x\n",p,ADU.data[p]);
						p++;
						databu--;
					}
				}
			}

			u3_Rxch = 0;
			HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//启动串口中断接收，必须的（准备接受下次数据）
		}
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
