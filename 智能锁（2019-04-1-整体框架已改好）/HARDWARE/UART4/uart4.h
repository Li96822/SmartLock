#ifndef __USART3_H
#define __USART3_H
#include "sys.h"

#define UART_REC_LEN  			200
extern unsigned char RxCounter2,RxBuffer2[50];

void uart4Init(void);
void UART4_sendcmd_byte(char data);
void Uart4_SendCmdStr(char *SendBuf);
void UART4_senddata_byte(unsigned char data);
void Uart4_SendDataStr(unsigned char *SendBuf,unsigned char len);
#define EN_UART4_RX 			1		//使能（1）/禁止（0）串口1接收
//#define UART4_MAX_RECV_LEN		400					//最大接收缓存字节数
//#define UART4_MAX_SEND_LEN		400					//最大发送缓存字节数	
//#define UART4_RX_EN 			1					//0,不接收;1,接收.
#endif
