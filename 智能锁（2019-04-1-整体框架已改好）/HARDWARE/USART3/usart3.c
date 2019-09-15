#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "oled.h"
#include "usart3.h"
u8 USART_RX_BUF1[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节
u16 USART_RX_STA1=0;       //接收状态标记	  
u8 RxBuffer1[500]={0x00};
u8 Uart_send_counter; //Uart_send()函数发送的字节数
vu8 *Uart_send_pointer;//发送的数组指针
u32 RxCounter1=0;
u8 flag_nfc_status;
u8 flag_rev_finish;  //1为接收完成


unsigned char RxCounter,RxBuffer[600];     

void UART3_sendcmd_byte(char data)
{
	while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	USART_SendData(USART3, data);
}
void UART1_sendcmd_byte(char data)
{
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, data);
}
void Uart1_SendCmdStr(char *SendBuf)
{
	while(*SendBuf)
	{
	  UART1_sendcmd_byte(*SendBuf);
		SendBuf++;
	}
}

void Uart3_SendCmdStr(char *SendBuf)
{
	while(*SendBuf)
	{
	  UART3_sendcmd_byte(*SendBuf);
		SendBuf++;
	}
}
void UART3_senddata_byte(unsigned char data)
{
    while(USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
	  USART_SendData(USART3, data);
}

void Uart3_SendDataStr(unsigned char *SendBuf,unsigned int len)
{
    int i=0;
	  for(i=0;i<len;i++)
	  {
		    UART3_senddata_byte(*SendBuf);
			  SendBuf++;
		}
}

void usart3Init()
{
    NVIC_InitTypeDef NVIC_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
 	  USART_DeInit(USART3);  
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOB, &GPIO_InitStructure); 
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);  
    GPIO_PinRemapConfig(GPIO_PartialRemap_USART3,ENABLE);
	
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;		
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	  NVIC_Init(&NVIC_InitStructure);	
  
	  USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx |USART_Mode_Tx;	

    USART_Init(USART3, &USART_InitStructure); 
    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);                    
}

void USART3_IRQHandler(void)                	
{
	  

    if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)
		{
		     RxBuffer[RxCounter++] = USART_ReceiveData(USART3);
			   
		}
		USART_ClearITPendingBit(USART3, USART_IT_RXNE);
	
} 































