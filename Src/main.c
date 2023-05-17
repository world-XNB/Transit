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
uint8_t IDENTIFIER = 0x10;		//С����ʶ������ʱΪһ��

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
uint8_t u1_Rxch=1;

uint8_t u3_Rxlenth=0;
uint8_t u3_Rxbuff[255];
uint8_t u3_Rxch=0;

uint8_t recvflag = 0;	//usart3���ܻص�������ı�־��0�������ʱ���ܵ���ATָ���־��1�������ʱ���ܵ��Ƿ��������ݣ�͸��ģʽ��

struct MDBOS_ADU ADU;

uint8_t recving_ADU_flag = 0;	//�Ƿ����ڽ���MODBUS_ADU��0����1���ǡ�
uint8_t lenthbuff = 6;	//������ʶ��ʱMODBUS_ADU_head+fun_code�ж����ֽ�����,��ʼ��Ϊ6,�ȴ�ʱ��Ҫ����6�ֽ����ݣ�2�ֽ�Э���ʶ����2�ֽں����ֽڳ��ȡ�1�ֽڵ�Ԫ��ʶ����1�ֽڹ����룩
uint16_t ADU_head_lenth = 0;	//�����洢MODBUS_ADU_head_lenth
uint8_t ADU_data_pos = 0;	//������ʶADU_data���±꣬������㿪ʼ
uint8_t data_buff[1024];
uint8_t ADU_data_lenth = 0;

uint8_t FunCode = 0x00;	//С�������洢MODBUS��������м����
uint8_t Carspeed = 0;
uint8_t Pstation = 0;
uint8_t Pdistance = 0;
uint8_t cardata[3];	//��¼������ʱ״̬


uint16_t last_count;
uint16_t curr_count;
uint8_t direct = 0;	//��������ת0����ת1
uint16_t overflow = 0;	//�����������־
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
	last_count = __HAL_TIM_GET_COUNTER(&htim3);	//��¼��ǰ����ֵ
	HAL_TIM_Base_Start_IT(&htim4);
	
	HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//��ǰ���������ж�׼�����ܷ���ATָ��֮��ķ�������
	HAL_UART_Receive_IT(&huart1,(uint8_t *)&u1_Rxch,1);	//���������жϽ��գ������
	
	Connect_Wifi();		//���Ӿ�����
	Connect_IP();	//����IP
	recvflag = 1;	  //��ʱ����Ϊ͸��ģʽ�µ����ݴ���
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  direct = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);	//��ȡ����������
	  if(FunCode == 0x00)
	  {
		  FollowTheTrack();
		  if(u1_Rxch == 0x00)	//����ָ��Ͽ�IP����
		  {
			  recvflag = 0;
			  Break_IP();
			  break;
		  }
		  else
			  HAL_UART_Receive_IT(&huart1,(uint8_t *)&u1_Rxch,1);	//���������жϽ��գ������
	  }
	  else
	  {
		  if(u1_Rxch == 0x00)	//����ָ��Ͽ�IP����
		  {
			  recvflag = 0;
			  Break_IP();
			  break;
		  }
		  else
			  HAL_UART_Receive_IT(&huart1,(uint8_t *)&u1_Rxch,1);	//���������жϽ��գ������
		  
		  switch(FunCode)
		  {
//			  case 0x00: break;
			  case 0x41: printf("��������С����ʱ״̬������\n");  CarStat(); Send_Data(cardata); FunCode = 0x00; break;
			  case 0x42: printf("���ڿ���С����%x\n",ADU.data[0]); modbusControl(ADU.data[0]); FunCode = 0x00; break;
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

//��ʱ���жϻص�����
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim4)
	{
		curr_count = __HAL_TIM_GET_COUNTER(&htim3);
		if(direct == 0)
		{
			count_temp = curr_count+(4000-last_count)+(overflow-1)*4000;	//������1s���յ����ź�
			count_temp=count_temp/14;	//������ת�٣����תһȦ14���źţ�
			Carspeed = count_temp/10 ;	//������ٱ�10
			Pstation = Pstation + Carspeed;
			
//			printf("direct:%d\tCarspeed:%x\t%d\n",direct,Carspeed,Carspeed);
		}
		else if(direct == 1)
		{
			count_temp = last_count+(4000-curr_count)+(overflow-1)*4000;	//������1s���յ����ź�
			count_temp=count_temp/14;	//������ת�٣����תһȦ14���źţ�
			Carspeed = count_temp/10;	//������ٱ�10
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

//USART�����жϻص�����
//@brief Transit callback function for USART3
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{	
    if(huart->Instance == USART3)//�жϴ��ں�
    {
		HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//���������жϽ��գ������
    }
}

//USART�����жϷ�����
//@brief Rewrite callback function for USART3
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{	
    if(huart->Instance == USART3)//�жϴ��ںš���usart3��������������
    {
		u3_Rxlenth++;
		u3_Rxbuff[u3_Rxlenth-1] = u3_Rxch;
		if(recvflag == 0)
		{
			if(u3_Rxch == 0x0A&&u3_Rxbuff[u3_Rxlenth-2] == 0x0D&&u3_Rxbuff[u3_Rxlenth-3] == 0x4B&&u3_Rxbuff[u3_Rxlenth-4] == 0x4F)	//������־ 0x4f 0x4b 0x0d 0x0a(��OK\r\n��)
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
			HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//���������жϽ��գ�����ģ�׼�������´����ݣ�
		}
		else if(recvflag == 1)
		{	
			if(recving_ADU_flag == 0)	//��ʱ���ܵ�����ͨ����
			{
				if(u3_Rxch == 0x00)	//������־ 0x0(��\0��)
				{
					printf("%s",u3_Rxbuff);
					memset(u3_Rxbuff,0,sizeof(u3_Rxbuff));
					u3_Rxlenth=0;
				}
				else if(u3_Rxch == 0x01&&u3_Rxbuff[u3_Rxlenth-2] == 0x15)	//��ȡMODBUS_ADU�������ʶ��
				{
					recving_ADU_flag = 1;
					ADU.head.TMIdent[1] = u3_Rxch;
					ADU.head.TMIdent[0] = u3_Rxbuff[u3_Rxlenth-2];
					
					memset(data_buff,0,sizeof(data_buff));
				}
			}	
			else	//��ʱ���ڽ���MODBUS_ADU
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
					
				if(ADU_data_lenth > 0)	//��ʱ��ʼ����ADU_data����
				{
					data_buff[ADU_data_pos] = u3_Rxch;
					ADU_data_pos++;
					
					if(ADU_data_lenth == 1)	//���ݽ��ܼ�������
					{				
						ADU.data = data_buff;	//��MODBUS_ADU_DATAָ�����ǵ����ݻ�����
						
						//Ϊ��һ�ν�����׼��
						memset(u3_Rxbuff,0,sizeof(u3_Rxbuff));

						u3_Rxlenth = 0;
						recving_ADU_flag = 0;
						lenthbuff = 6;
						ADU_data_pos = 0;	
						ADU_head_lenth = 0;
					}
					
					ADU_data_lenth--;
				}
				else if(ADU_head_lenth == 2)	//����ADU
				{			
					//Ϊ��һ�ν�����׼��
					memset(u3_Rxbuff,0,sizeof(u3_Rxbuff));
					
					u3_Rxlenth = 0;
					recving_ADU_flag = 0;
					lenthbuff = 6;
					ADU_data_pos = 0;
					ADU_head_lenth = 0;
				}
				
				if(recving_ADU_flag == 0)	//������ɣ���ӡ
				{
					FunCode = ADU.fun_code;	//	��ע�˴�MODBUS������
					
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
			HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//���������жϽ��գ�����ģ�׼�������´����ݣ�
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
