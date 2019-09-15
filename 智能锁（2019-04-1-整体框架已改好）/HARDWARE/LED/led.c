#include "led.h"

 

//这里为什么不是定义PE.5和PB.5？ ------  貌似主程序中没有用到

//LED IO初始化
void LED_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				 //LED 端口配置
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE.2
 GPIO_SetBits(GPIOE,GPIO_Pin_2);						 //PE2 输出高


}
 
void L620_CONTROLInit(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	 //使能PE端口时钟
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;				       //POWERKEY
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE.13
 GPIO_SetBits(GPIOE,GPIO_Pin_13);						 //PE13 输出高

 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;				     //RSTKEY
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
 //GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
 GPIO_Init(GPIOE, &GPIO_InitStructure);					 //根据设定参数初始化GPIOE.14
 GPIO_SetBits(GPIOE,GPIO_Pin_14);						 //PE14 输出高
	
	
}
