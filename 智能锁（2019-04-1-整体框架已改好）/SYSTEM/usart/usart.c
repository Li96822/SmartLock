#include "sys.h"
#include "usart.h"	 
#include "time.h"
#include "stmflash.h"
#include "delay.h"
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 


 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN]={0x00};     //���ջ���,���USART_REC_LEN���ֽ�.
u8 RxCount2=0,RxCount1=0;
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	 
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.
u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 			  //���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
vu16 USART2_RX_STA=0;   

void USART1_Int(u16 baud)
{

//GPIO�˿�����
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef  USART_ClockInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
		
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);// Tx

 
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;			
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;			
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;			
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;		
	/* Configure the USART2 synchronous paramters */
	USART_ClockInit(USART1, &USART_ClockInitStructure);			 
	USART_InitStructure.USART_BaudRate = baud;				 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		 
	USART_InitStructure.USART_Parity = USART_Parity_No ;			 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  
	/* Configure USART2 basic and asynchronous paramters */
	USART_Init(USART1, &USART_InitStructure);
      /* Enable USART2 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��NVIC�Ĵ���
	
	//USART_ClearFlag(USART2, USART_IT_RXNE); 			
	USART_ITConfig(USART1,USART_IT_RXNE, ENABLE);		
	//USART_ITConfig(USART2, USART_IT_TC, ENABLE);	
	USART_Cmd(USART1, ENABLE);	
     //ʹ�ܴ���   
}
void USART2_Int(u32 baud)
{

    //GPIO�˿�����
  USART_InitTypeDef USART_InitStructure;
  USART_ClockInitTypeDef  USART_ClockInitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
		
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA,&GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);// Tx

 
	USART_ClockInitStructure.USART_Clock = USART_Clock_Disable;			
	USART_ClockInitStructure.USART_CPOL = USART_CPOL_Low;			
	USART_ClockInitStructure.USART_CPHA = USART_CPHA_2Edge;			
	USART_ClockInitStructure.USART_LastBit = USART_LastBit_Disable;		
	/* Configure the USART2 synchronous paramters */
	USART_ClockInit(USART2, &USART_ClockInitStructure);			 
	USART_InitStructure.USART_BaudRate = baud;				 
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	 
	USART_InitStructure.USART_StopBits = USART_StopBits_1;		 
	USART_InitStructure.USART_Parity = USART_Parity_No ;			 
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	  
	/* Configure USART2 basic and asynchronous paramters */
	USART_Init(USART2, &USART_InitStructure);
      /* Enable USART2 */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
	//USART_ClearFlag(USART2, USART_IT_RXNE); 			
	USART_ITConfig(USART2,USART_IT_RXNE, ENABLE);		
	//USART_ITConfig(USART2, USART_IT_TC, ENABLE);	
	USART_Cmd(USART2, ENABLE);	
     //ʹ�ܴ���  
  TIM7_Int_Init(99,7199);		//10ms�ж�
	USART2_RX_STA=0;		//����
	TIM_Cmd(TIM7,DISABLE);			//�رն�ʱ��7		 
}



void USART2_IRQHandler(void)
{
	
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
	    USART2_RX_BUF[RxCount2++] = USART_ReceiveData(USART2);
	}
	    
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);								 
}  


void UART1_Send_Str(char *s)
{
 u8 i=0;
 while(s[i]!='\0')
 {
  USART_SendData(USART1,s[i]);
	delay_ms(50);
  while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
  i++;
 }
}

void UART2_Send_Str(char *s)
{
 u8 i=0;
 while(s[i]!='\0')
 {
  USART_SendData(USART2,s[i]);
  while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
  i++;
 }
}

void UART1_Send_char(unsigned char s)
{
 USART_SendData(USART1,s);
 while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
}

void UART2_Send_char(unsigned char s)
{
 USART_SendData(USART2,s);
 while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);
}
void Usart1_Send(u8 *buf,u8 len)
{
 u8 t;
 for(t=0;t<len;t++)
 {
  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
  USART_SendData(USART1,buf[t]);
	delay_ms(10);
 }
  while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);
}

void Usart2_Send(u8 *buf,u8 len)
{
 u8 t;
 for(t=0;t<len;t++)
 {
  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
  USART_SendData(USART2,buf[t]);
 }
  while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);
}
void USART1_IRQHandler(void)                	
{
	  u8 res=0;
		if(USART_GetITStatus(USART1, USART_IT_RXNE)!= RESET)  
		{
        res=USART_ReceiveData(USART1);	
        if(RxCount1==0&&(res==0xff||res==0x00))
        {
	         RxCount1=0;	
        }
        else 
        {
	         USART_RX_BUF[RxCount1]=res;
	         RxCount1++;
        }			
    }		
    USART_ClearITPendingBit(USART1, USART_IT_RXNE);		
} 	 
#endif	

