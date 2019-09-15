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
extern unsigned char f1,f2,f3;   //�������·��û����ݣ�NB���ձ�־λ
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
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//��ձ�־λ
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
		TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
		TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
		TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
		//TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

		TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0 ; //�����ȼ�3��
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
		NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

		TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx����
							 
}
void TIM2_IRQHandler(void)   //TIM2�ж϶�ʱ�ϴ��û���Ϣ����״̬
{

	
	unsigned int u=0,v;
	unsigned char flag;
	if(TIM_GetITStatus(TIM2,TIM_IT_Update) != RESET)//��ձ�־λ
	{ 
		if(k1==1)
		{
		   k1=0;
			 if(CorrectFlag==1)    
			 {  
				 while(flag==0)				//����L620NBģ��
					{
							Uart3_SendCmdStr("AT\r\n"); 
							delay_ms(1000);
							flag=L620Match(RxBuffer,r1,2);
							Clear_Buffer();	
					}
					UART1_Send_char(0x01);//���ѱ�־

			   STMFLASH_Read(FLASH_USERMESSAGE_ADDR1,(u16*)UserMessage1,Correctnum*10+6);  //��flash�ж�ȡ�û�����״̬��Ϣ
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
			 
			   CorrectFlag=0;                         //�޸ļ�¼��־λ����
			   addr=FLASH_USERMESSAGE_ADDR1+6;        //��ַ���¶�λ��ͷ6���ֽ��ȴ洢��ͷ5��Ϊ��Ч�ֽڣ�
			 
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
				 UART1_Send_char(0x09);//�������·�ָ���־
				 Correctnum=0;
//				 TIM_Cmd(TIM3, ENABLE);  //ʹ��TIM3����
				
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
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//��ձ�־λ	  	
	}
}
void TIM3_Int_Init(u32 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//��ձ�־λ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1 ; //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx����
}
void TIM3_IRQHandler(void)   //TIM3�ж϶�ʱ�ϴ��û�������¼
{ 
	
	unsigned int r=0,s;
	unsigned char flag;
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET)//��ձ�־λ
	{
	   if(m1==1)
		 {
		   m1=0;
			 if(RecordFlag==1)
			 { 
					while(flag==0)		//����L620NBģ��
					{
							Uart3_SendCmdStr("AT\r\n"); 
							delay_ms(1000);
							flag=L620Match(RxBuffer,r1,2);
							Clear_Buffer();	
					}
					UART1_Send_char(0x01);//���ѱ�־
			 STMFLASH_Read(FLASH_USERMESSAGE_ADDR,(u16*)UserMessage2,Recordnum*10+6);     //�ڴ��ܹ�ռ��Recordnum*10+6,ʵ����Ч��ΪRecordnum*9+5
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

			 RecordFlag=0;                   //������¼��־����
			 addr1=FLASH_USERMESSAGE_ADDR+6;   //��ַ���¶�λ��ͷ6���ֽ��ȴ洢��ͷ5��Ϊ��Ч�ֽڣ�
			 
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
		 TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//��ձ�־λ
	}
}
//ͨ�ö�ʱ��7�жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ42M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz 
//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM7_Int_Init(u16 arr,u16 psc)
{	
	NVIC_InitTypeDef NVIC_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE);//TIM7ʱ��ʹ��    
	
	//��ʱ��TIM7��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM7�ж�,��������ж�
	
	TIM_Cmd(TIM7,ENABLE);//������ʱ��7
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
	
}
//��ʱ��7�жϷ������		    
void TIM7_IRQHandler(void)
{ 	
	if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET)//�Ǹ����ж�
	{	 			   
		USART2_RX_STA|=1<<15;	//��ǽ������
		TIM_ClearITPendingBit(TIM7, TIM_IT_Update  );  //���TIM7�����жϱ�־    
		TIM_Cmd(TIM7, DISABLE);  //�ر�TIM7 
	}	    
}
 	 

void TIM4_Int_Init(u32 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//��ձ�־λ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
  TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	 ������5000Ϊ500ms
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ  10Khz�ļ���Ƶ��  
	//TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4, TIM_IT_Update,ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =1;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2 ; //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

	//TIM_Cmd(TIM4, ENABLE);  //ʹ��TIMx����
}
void TIM4_IRQHandler(void)      //��ʱ2��Զ�̿���
{
	//unsigned char mm;
	unsigned int i;
	
	if(TIM_GetITStatus(TIM4,TIM_IT_Update) != RESET)//��ձ�־λ
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
		 
		 TIM_ClearITPendingBit(TIM4,TIM_IT_Update);//��ձ�־λ
	
	 }
}











