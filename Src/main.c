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
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t u1_Rxch=1;

uint8_t u3_Rxlenth=0;
uint8_t u3_Rxbuff[255];
uint8_t u3_Rxch=0;

uint8_t recvflag = 0;	//usart3接受回调函数里的标志。0：代表此时接受的是AT指令标志。1：代表此时接受的是服务器数据（透传模式）

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

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
  /* USER CODE BEGIN 2 */
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
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
	  Send_Data();
	  if(u1_Rxch == 0x00)
	  {
		  recvflag = 0;
		  Break_IP();
		  break;
	  }
	  else
		  HAL_UART_Receive_IT(&huart1,(uint8_t *)&u1_Rxch,1);	//启动串口中断接收，必须的
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
		u3_Rxbuff[u3_Rxlenth-1]=u3_Rxch;
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
			HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//启动串口中断接收，必须的
		}
		else if(recvflag == 1)
		{
			if(u3_Rxch == 0x00)	//结束标志 0x0(“\0”)
			{
				printf("%s",u3_Rxbuff);
				memset(u3_Rxbuff,0,sizeof(u3_Rxbuff));
				u3_Rxlenth=0;
			}
			u3_Rxch = 0;
			HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//启动串口中断接收，必须的
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
