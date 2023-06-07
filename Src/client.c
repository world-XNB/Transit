#include "client.h"

extern uint8_t u3_Rxbuff[2];
extern uint8_t u3_Rxch;


void Connect_Wifi(void)
{
//	连接局域网
	uint8_t AT_RST[] = "AT+RST\r\n";
	uint8_t AT[] = "AT\r\n";
	uint8_t AT_CWMODE[]="AT+CWMODE=1\r\n";
	uint8_t AT_CWJAP[]="AT+CWJAP=\"GZGD-NkHi\",\"96789783\"\r\n";
	uint8_t AT_CIFSR[]="AT+CIFSR\r\n";
	uint8_t AT_CIPMUX[]="AT+CIPMUX=0\r\n";
	
//  将接收到的数据再通过串口发送出去(减1是为了减去字符串的结尾“\0”)
	HAL_UART_Transmit_IT(&huart3, AT_RST, sizeof(AT_RST)-1);	//复位AT
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT, sizeof(AT)-1);	//发送AT指令，验证AT指令是否有效
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CWMODE, sizeof(AT_CWMODE)-1);	  //将esp8266设置为STA模式
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CWJAP, sizeof(AT_CWJAP)-1);	//连接局域网
	HAL_Delay(10000);	//延时时间长一点，不然下一条指令发送时还没有接受到该指令返回信息
	HAL_UART_Transmit_IT(&huart3, AT_CIFSR, sizeof(AT_CIFSR)-1);	//查询在该局域网下esp8266分配的IP
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPMUX, sizeof(AT_CIPMUX)-1);	//设置为多连接模式
	HAL_Delay(1000);
	
}

//连接IP
void Connect_IP(void)
{
	uint8_t AT_CIPMODE1[]="AT+CIPMODE=1\r\n";
	uint8_t AT_CIPSEND[]="AT+CIPSEND\r\n";
	uint8_t AT_CIPSTART[]="AT+CIPSTART=\"TCP\",\"192.168.1.5\",502\r\n";

	HAL_UART_Transmit_IT(&huart3, AT_CIPSTART, sizeof(AT_CIPSTART)-1);	//连接服务器IP
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPMODE1, sizeof(AT_CIPMODE1)-1);	//进入透传模式
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPSEND, sizeof(AT_CIPSEND)-1);	//进入数据传输
	HAL_Delay(1000);
}

//断开IP
void Break_IP(void)
{
	uint8_t OVER[]="+++";
	uint8_t AT_CIPCLOSE[]="AT+CIPCLOSE\r\n";
	uint8_t AT_CIPMODE0[]="AT+CIPMODE=0\r\n";
	
	HAL_UART_Transmit_IT(&huart3, OVER, sizeof(OVER)-1);	//退出数据传输
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPMODE0, sizeof(AT_CIPMODE0)-1);	//关闭透传模式
	HAL_Delay(1000);
	HAL_UART_Transmit_IT(&huart3, AT_CIPCLOSE, sizeof(AT_CIPCLOSE)-1);	//关闭服务器IP连接
	HAL_Delay(1000);
}

void Send_Data(uint8_t *data,int lenth)
{	
	//	数据传输
	HAL_UART_Transmit_IT(&huart3, data, lenth);	
//	HAL_Delay(10000);
//	HAL_UART_Receive(&huart3,(uint8_t *)&u3_Rxch,1,100000);	//启动串口中断接收，必须的
	HAL_UART_Receive_IT(&huart3,(uint8_t *)&u3_Rxch,1);	//提前启动串口中断准备接受发送AT指令之后的返回数据
}

