#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "oled.h"
#include "uart4.h"
#include "usart.h"   
unsigned char RxCounter2,RxBuffer2[50];

void UART4_sendcmd_byte(char data)
{
	while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);
	USART_SendData(UART4, data);
}

void Uart4_SendCmdStr(char *SendBuf)
{
	while(*SendBuf)
	{
	  UART4_sendcmd_byte(*SendBuf);
		SendBuf++;
	}
}
void UART4_senddata_byte(unsigned char data)
{
		USART_SendData(UART4, data);
    while(USART_GetFlagStatus(UART4, USART_FLAG_TXE) == RESET);

}

void Uart4_SendDataStr(unsigned char *SendBuf,unsigned char len)
{
    int i=0;
	  for(i=0;i<len;i++)
	  {
		    UART4_senddata_byte(*SendBuf);
			 // UART1_Send_char(0x05);
			  delay_ms(1);
			  SendBuf++;
		}
}

void uart4Init()
{
    NVIC_InitTypeDef NVIC_InitStructure;
	  GPIO_InitTypeDef GPIO_InitStructure;
	  USART_InitTypeDef USART_InitStructure;
	
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
	  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	
 	  USART_DeInit(UART4);  
	
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOC, &GPIO_InitStructure); 
   
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);  
    //GPIO_PinRemapConfig(GPIO_PartialRemap_UART4,ENABLE);
	
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			
	  NVIC_Init(&NVIC_InitStructure);	
  
	  USART_InitStructure.USART_BaudRate = 115200;
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;
		USART_InitStructure.USART_StopBits = USART_StopBits_1;
		USART_InitStructure.USART_Parity = USART_Parity_No;
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
		USART_InitStructure.USART_Mode = USART_Mode_Rx |USART_Mode_Tx;	

    USART_Init(UART4, &USART_InitStructure); 
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
    USART_Cmd(UART4, ENABLE);                    
}

void UART4_IRQHandler(void)                	
{
    if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
		{
		     RxBuffer2[RxCounter2++] = USART_ReceiveData(UART4);
		}
		USART_ClearITPendingBit(UART4, USART_IT_RXNE);
}
































