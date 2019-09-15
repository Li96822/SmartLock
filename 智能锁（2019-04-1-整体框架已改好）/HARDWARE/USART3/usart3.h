#ifndef __USART3_H
#define __USART3_H

#include "sys.h"
#define USART_REC_LEN  			200
extern unsigned char RxCounter,RxBuffer[600];
void usart3Init(void);
void TIM2_Configuration(void);
void UART3_sendcmd_byte(char data);
void Uart3_SendCmdStr(char *SendBuf);
void UART1_sendcmd_byte(char data);
void Uart1_SendCmdStr(char *SendBuf);
void UART3_senddata_byte(unsigned char data);
void Uart3_SendDataStr(unsigned char *SendBuf,unsigned int len);

#define EN_USART3_RX  1     //使能（1）/禁止（0）串口1接收
	  	
//u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
//u16 USART_RX_STA;         		//接收状态标记	


#endif
