#include "key.h"
#include "usart.h"
#include "stm32f10x.h"
#include "delay.h"
u8 keyvalue;
//u32 readvalue;
u8 t;

void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF|RCC_APB2Periph_AFIO, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//列 推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  //行 上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOF, &GPIO_InitStructure);
	
	GPIO_SetBits(GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
	GPIO_ResetBits(GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
	
}

void fingler(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

	GPIO_Init(GPIOA, &GPIO_InitStructure);
	 GPIO_ResetBits(GPIOA,GPIO_Pin_6);
}

u8 keyScan(u8 mode)
{	 
	  u8 KeyValue=0xff;
	  static u8 key_up=1;
	  
	  if(mode) key_up=1;
	  
		if(key_up&&(GPIO_ReadInputData(GPIOF)&0x000f)!=0x000f)
		{
		//UART1_Send_char(0x01);
		 delay_ms(5);
		 key_up=0;
		 if((GPIO_ReadInputData(GPIOF)&0x000f)!=0x000f)
		 {
			//UART1_Send_char(0x02);
		  GPIO_SetBits(GPIOF,GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
			GPIO_ResetBits(GPIOF,GPIO_Pin_4);
			switch((GPIO_ReadInputData(GPIOF)&0x000f))
			{
				case 0x000e: KeyValue= 1;break;
				case 0x000D: KeyValue= 4;break;
				case 0x000B: KeyValue= 7;break;
				case 0x0007: KeyValue=14;break;
			}
			
		 
		  GPIO_SetBits(GPIOF,GPIO_Pin_4|GPIO_Pin_6|GPIO_Pin_7);
			GPIO_ResetBits(GPIOF,GPIO_Pin_5);
			switch((GPIO_ReadInputData(GPIOF)&0x000f))
			{
				case 0x000e: KeyValue= 2;break;
				case 0x000D: KeyValue= 5;break;
				case 0x000B: KeyValue= 8;break;
				case 0x0007: KeyValue= 0;break;
			}
			
		 
		 
		  GPIO_SetBits(GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_7);
			GPIO_ResetBits(GPIOF,GPIO_Pin_6);
			switch((GPIO_ReadInputData(GPIOF)&0x000f))
			{
				case 0x000e: KeyValue= 3;break;
				case 0x000D: KeyValue= 6;break;
				case 0x000B: KeyValue= 9;break;
				case 0x0007: KeyValue=15;break;
			}
			
/*		 
		  GPIO_SetBits(GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6);
			GPIO_ResetBits(GPIOF,GPIO_Pin_7);
			switch((GPIO_ReadInputData(GPIOF)&0x000f))
			{
				case 0x000e: KeyValue=10;break;
				case 0x000D: KeyValue=11;break;
				case 0x000B: KeyValue=12;break;
				case 0x0007: KeyValue=13;break;
			}
*/			
			GPIO_SetBits(GPIOF,GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3);
			GPIO_ResetBits(GPIOF,GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7);
			return KeyValue;
		 }
		}
		else if((GPIO_ReadInputData(GPIOF)&0x000f)==0x000f)
		{
		 key_up=1;
		 //UART1_Send_char(0x03);
		}
		return 0xff;
}































































