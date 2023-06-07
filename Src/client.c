#include "client.h"

extern uint8_t u3_Rxbuff[2];
extern uint8_t u3_Rxch;


void Connect_Wifi(void)
{
//	���Ӿ�����
	uint8_t AT_RST[] = "AT+RST\r\n";
	uint8_t AT[] = "AT\r\n";
	uint8_t AT_CWMODE[]="AT+CWMODE=1\r\n";
	uint8_t AT_CWJAP[]="AT+CWJAP=\"GZGD-NkHi\",\"96789783\"\r\n";
	uint8_t AT_CIFSR[]="AT+CIFSR\r\n";
	uint8_t AT_CIPMUX[]="AT+CIPMUX=0\r\n";
	
//  �����յ���������ͨ�����ڷ��ͳ�ȥ(��1��Ϊ�˼�ȥ�ַ����Ľ�β��\0��)
	HAL_UART_Transmit_IT(&huart3, AT_RST, sizeof(AT_RST)-1);	//��λAT
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT, sizeof(AT)-1);	//����ATָ���֤ATָ���Ƿ���Ч
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CWMODE, sizeof(AT_CWMODE)-1);	  //��esp8266����ΪSTAģʽ
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CWJAP, sizeof(AT_CWJAP)-1);	//���Ӿ�����
	HAL_Delay(10000);	//��ʱʱ�䳤һ�㣬��Ȼ��һ��ָ���ʱ��û�н��ܵ���ָ�����Ϣ
	HAL_UART_Transmit_IT(&huart3, AT_CIFSR, sizeof(AT_CIFSR)-1);	//��ѯ�ڸþ�������esp8266�����IP
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPMUX, sizeof(AT_CIPMUX)-1);	//����Ϊ������ģʽ
	HAL_Delay(1000);
	
}

//����IP
void Connect_IP(void)
{
	uint8_t AT_CIPMODE1[]="AT+CIPMODE=1\r\n";
	uint8_t AT_CIPSEND[]="AT+CIPSEND\r\n";
	uint8_t AT_CIPSTART[]="AT+CIPSTART=\"TCP\",\"192.168.1.5\",502\r\n";

	HAL_UART_Transmit_IT(&huart3, AT_CIPSTART, sizeof(AT_CIPSTART)-1);	//���ӷ�����IP
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPMODE1, sizeof(AT_CIPMODE1)-1);	//����͸��ģʽ
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPSEND, sizeof(AT_CIPSEND)-1);	//�������ݴ���
	HAL_Delay(1000);
}

//�Ͽ�IP
void Break_IP(void)
{
	uint8_t OVER[]="+++";
	uint8_t AT_CIPCLOSE[]="AT+CIPCLOSE\r\n";
	uint8_t AT_CIPMODE0[]="AT+CIPMODE=0\r\n";
	
	HAL_UART_Transmit_IT(&huart3, OVER, sizeof(OVER)-1);	//�˳����ݴ���
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPMODE0, sizeof(AT_CIPMODE0)-1);	//�ر�͸��ģʽ
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPCLOSE, sizeof(AT_CIPCLOSE)-1);	//�رշ�����IP����
	HAL_Delay(1000);
}

void Send_Data(uint8_t *data,int lenth)
{	
	//	���ݴ���
	HAL_UART_Transmit_IT(&huart3, data, lenth);	
//	HAL_Delay(10000);
//	HAL_UART_Receive(&huart3,(uint8_t *)&u3_Rxch,1,100000);	//���������жϽ��գ������
	HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//��ǰ���������ж�׼�����ܷ���ATָ��֮��ķ�������
}

