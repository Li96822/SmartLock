#include "wkup.h"
#include "led.h"
#include "delay.h"
#include "key.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK战舰STM32开发板
//待机唤醒 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2012/9/7
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
//u8 t;	 
void Sys_Standby(void)
{  
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);	//使能PWR外设时钟
	PWR_WakeUpPinCmd(ENABLE);  //使能唤醒管脚功能
	PWR_EnterSTANDBYMode();	  //进入待命（STANDBY）模式 		 
}
//系统进入待机模式
void Sys_Enter_Standby(void)
{			 
	RCC_APB2PeriphResetCmd(0X01FC,DISABLE);	//复位所有IO口
	Sys_Standby();
}
//检测WKUP脚的信号
//返回值1:连续按下3s以上
//      0:错误的触发	
u8 Check_WKUP(void) 
{
	u8 ti=0;
	u8 tx=0;//记录松开的次数
//	LED0=0; //亮灯DS0 
	while(1)
	{
		t=keyScan(0);
		if(t!=0xff)//已经按下了
		{
			ti++;
			tx=0;
		    delay_ms(30);
		   if(ti>=100)//按下超过3秒钟
			{
			//	LED0=0;	  //点亮DS0 
				t=0xff;
				return 1; //按下3s以上了
			}
		 }
		else 
		   {
				tx++; //超过300ms内没有WKUP信号
			    delay_ms(30);
				if(tx>=100)
				{
				//	LED0=1;
					return 0;//错误的按键,按下次数不够
				}
		   }
	
	}
}  
//中断,检测到PA0脚的一个上升沿.	  
//中断线0线上的中断检测


void EXTI0_IRQHandler(void)
{ 		    		    				     		    
	EXTI_ClearITPendingBit(EXTI_Line0); // 清除LINE10上的中断标志位		  
	if(Check_WKUP())//关机?
	{		  
		Sys_Enter_Standby();  
	}
} 
//PA0 WKUP唤醒初始化
void WKUP_Init(void)
{	
//    GPIO_InitTypeDef GPIO_InitStructure;  		  
	NVIC_InitTypeDef NVIC_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOF | RCC_APB2Periph_AFIO, ENABLE);//使能GPIOA和复用功能时钟

//	GPIO_InitStructure.GPIO_Pin =GPIO_Pin_15;	 //PA.0
//	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;//浮空输入（高电平）
//	GPIO_Init(GPIOE, &GPIO_InitStructure);	//初始化IO

    //使用外部中断方式
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3);	//中断线0连接GPIOE.0
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;			//设外外部中断模式:EXTI线路为中断请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  //上升沿触发
   	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOF,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7);	//中断线0连接GPIOE.0
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;	//设置按键所有的外部线路
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//设外外部中断模式:EXTI线路为中断请求
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  //下路沿触发
	
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	// 初始化外部中断
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; //使能按键所在的外部中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; //从优先级2级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	if(Check_WKUP()==0) Sys_Standby();    //不是开机,进入待机模式  
}

