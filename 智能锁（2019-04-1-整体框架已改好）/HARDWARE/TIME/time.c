#include "time.h"
#include "stmflash.h"
#include "break.h"
#include "L620.h"
#include "usart3.h"
#include "led.h"
#include "delay.h"
#include "oled.h"
#include "usart.h"
extern unsigned char r1[2];//OK
extern vu16 USART2_RX_STA;
extern unsigned char f1,f2,f3;   //服务器下发用户数据，NB接收标志位
u8 UserMessage1[400],UserMessage2[400];
u8 UnlockRecord[400]={0x00};
u8 CorrectRecord[400]={0x00};
unsigned char mm=0;
u8 m1=0,k1=0;
u16 n1=0,l1=0;
void TIM2_Int_Init(u32 arr,u16 psc)
{
		TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清空标志位
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
		TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
		//TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位

		TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0 ; //从优先级3级
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
		NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

		TIM_Cmd(TIM2, ENABLE);  //使能TIMx外设
							 
}
void TIM2_IRQHandler(void)   //TIM2中断定时上传用户信息更新状态
{

	
	unsigned int u=0,v;
	unsigned char flag;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)//清空标志位
	{ 
		if(k1==1)
		{
		   k1=0;
			 if(CorrectFlag==1)    
			 {  
				 while(flag==0)				//唤醒L620NB模块
					{
							Uart3_SendCmdStr("AT\r\n"); 
							delay_ms(1000);
							flag=L620Match(RxBuffer,r1,2);
							Clear_Buffer();	
					}
					UART1_Send_char(0x01);//唤醒标志

			   STMFLASH_Read(FLASH_USERMESSAGE_ADDR1,(u16*)UserMessage1,Correctnum*10+6);  //从flash中读取用户更新状态信息
				 for(v=0;v<Correctnum*10+6;v++)
			   {
				  if((v==5)||(v%10==5))
						 continue;
				  else
						 CorrectRecord[u++]=UserMessage1[v];
			   }
			   
			   Uart3_SendCmdStr("AT+CIPSEND=400\r\n");
			   delay_ms(1000);
				 CorrectRecord[u]=0XFF;
				 CorrectRecord[u+1]=0XFF;
			   Uart3_SendDataStr(CorrectRecord,400);
			 
			   CorrectFlag=0;                         //修改记录标志位清零
			   addr=FLASH_USERMESSAGE_ADDR1+6;        //地址重新定位（头6个字节先存储，头5个为有效字节）
			 
				 f1=receivedData();
	       f2=getUserMessage();
	       f3=delUserMessage();
	       delay_ms(1000);
				 
				 while((f1==0)&&(f2==0)&&(f3==0))
		     {
	         f1=receivedData();
           f2=getUserMessage();
			     f3=delUserMessage();
           delay_ms(1000);					
		     }
				 UART1_Send_char(0x09);//服务器下发指令标志
				 Correctnum=0;
//				 TIM_Cmd(TIM3, ENABLE);  //使能TIM3外设
				
			 }
		}
		else
		{
			if(l1==2)
			{
			   l1=0;
         k1++;				
			}
			else
				 l1++;
		}
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清空标志位	  	
	}
}
void TIM3_Int_Init(u32 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清空标志位
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1 ; //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx外设
}
void TIM3_IRQHandler(void)   //TIM3中断定时上传用户开锁记录
{ 
	
	unsigned int r=0,s;
	unsigned char flag;
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)//清空标志位
	{
	   if(m1==1)
		 {
		   m1=0;
			 if(RecordFlag==1)
			 { 
					while(flag==0)		//唤醒L620NB模块
					{
							Uart3_SendCmdStr("AT\r\n"); 
							delay_ms(1000);
							flag=L620Match(RxBuffer,r1,2);
							Clear_Buffer();	
					}
					UART1_Send_char(0x01);//唤醒标志
			 STMFLASH_Read(FLASH_USERMESSAGE_ADDR,(u16*)UserMessage2,Recordnum*10+6);     //内存总共占用Recordnum*10+6,实际有效的为Recordnum*9+5
			 for(s=0;s<Recordnum*10+6;s++)
			 {
				  if((s==5)||(s%10==5))
						 continue;
				  else
						 UnlockRecord[r++]=UserMessage2[s];
			 }
			 
			 Uart3_SendCmdStr("AT+CIPSEND=400\r\n");
			 delay_ms(1000);
			 CorrectRecord[r]=0XFF;
			 CorrectRecord[r+1]=0XFF;
			 Uart3_SendDataStr(UnlockRecord,400);

			 RecordFlag=0;                   //开锁记录标志清零
			 addr1=FLASH_USERMESSAGE_ADDR+6;   //地址重新定位（头6个字节先存储，头5个为有效字节）
			 
			 f1=receivedData();
	     f2=getUserMessage();
	     f3=delUserMessage();
	     delay_ms(1000);
			 while((f1==0)&&(f2==0)&&(f3==0))
		   {
				 f1=receivedData();
				 f2=getUserMessage();
				 f3=delUserMessage();
				 delay_ms(1000);					
		    }
			 
			 f1=0,f2=0,f3=0;
			 UART1_Send_char(0x09);
			 Recordnum=0;
//			 TIM_Cmd(TIM3, DISABLE);
			}
		 }
		 else
		 {  
			 if(n1==2)
			 {
		      n1=0;
				  m1++;
			 }
			 else
				  n1++;
		 }
		 TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清空标志位
	}
}
//通用定时器7中断初始化
//这里时钟选择为APB1的2倍，而APB1为42M
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz 
//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM7_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7时钟使能    
	
	//定时器TIM7初始化
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //使能指定的TIM7中断,允许更新中断
	
	TIM_Cmd(TIM7,ENABLE);//开启定时器7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
	
}
//定时器7中断服务程序		    
void TIM7_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//是更新中断
	{	 			   
		USART2_RX_STA|=1<<15;	//标记接收完成
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //清除TIM7更新中断标志    
		TIM_Cmd(TIM7, DISABLE);  //关闭TIM7 
	}	    
}
 	 

void TIM4_Int_Init(u32 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//清空标志位
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	 计数到5000为500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值  10Khz的计数频率  
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4, TIM_IT_Update,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2 ; //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

	//TIM_Cmd(TIM4, ENABLE);  //使能TIMx外设
}
void TIM4_IRQHandler(void)      //延时2秒远程开锁
{
	//unsigned char mm;
	unsigned int i;
	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)//清空标志位
	{
	   if(mm==1)
	   {
	      mm=0;
		    for(i=0;i<600;i++)
	      {
		       if((RxBuffer[i]==0x1f)&&(RxBuffer[i+1]==0x3d)&&(RxBuffer[i+2]==0x03))
           {
						 OLED_ShowString(0,36,"Unlock successfully",12); 
    	       OLED_Refresh_Gram();
		         delay_ns(5);
				     OLED_Clear();
						 Clear_Buffer();
						 TIM_Cmd(TIM4,DISABLE);	 
					   break;
			     }
		    }
		 
	   }
		 else
		   mm++;
		 
		 TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//清空标志位
	
	 }
}











