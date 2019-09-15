#include "led.h"
#include "delay.h"
#include "key.h"
#include "sys.h"
#include "oled.h"
#include "usart.h"
#include "stmflash.h"
#include "break.h"	
#include "usart3.h"
#include "as608.h"
#include "button.h"
#include "L620.h"
#include "rtc.h"
#include "time.h"
#include "id1016b.h"
#include "uart4.h"
#include "nfc.h"
#include "stmflash.h"
#include "wkup.h"
//#define I_Q PAout(6)

void uart4Init(void);
extern u32 addr,addr1;
//�����������ʽ
unsigned char buf1[7]={0x1f,0x3d,0x00,0x5b,0x50,0xff,0xff};//���ӷ�����
unsigned char buf2[]={0x1f,0x3d,0x01,0x91,0x80};  //�޸�����
unsigned char buf3[]={0x1f,0x3d,0x02,0x91,0x80};  //�޸�ָ��
unsigned char buf4[]={0x1f,0x3d,0xaa,0xff,0xff};  //Զ�̿���
unsigned char buf5[]={0x45,0x5b,0xce,0x2b};
unsigned char temp[]={0x00};

u8 table3[11]={0x00};
u8 table4[6]={0x00};
//unsigned char table13[]={0x00};
//unsigned char table14[]={0x00};
u8 tempdata1[11];
u8 table10[9]={0x00};
//unsigned char buf6[]={0xA2,0xCC,0xAA,0x39};
//unsigned char buf7[]={0xA3,0xB8,0xCB,0xE9};
u8 timebuffer[7]={0x00};
 
u8 table[9]={0x00};
//u8 Recordnum=0;
u8 len=0,m=0,ij=0,kk=0,bb=0;
u8 f1=0,f2=0,f3=0,f4=0,f5=0;

//u8 flag2=0;//flag2=flag1
//temp


u8 h=0;

 //   u8 Usercount=0;  //��¼�û���
//    u8 flag1=0;
    u8 P1[8];
	u8 n=0;
	u8 j=0,user_num=0;
    


int main(void)
{		

    u8 temp1,flag1=0;
//	u8 h;
//   	u8 mm=0;
	SystemInit();
	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	L620_CONTROLInit();
	
 	LED_Init();			     //LED�˿ڳ�ʼ��
	OLED_Init();			//��ʼ��OLED 
    WKUP_Init(); //�������ѳ�ʼ��
	OLED_ShowString(0,0,"Smart Lock",24);
	OLED_Refresh_Gram();
	delay_ms(1000);
//	
    USART1_Int(9600);
//	UART1_Send_char(0xFF);
	USART2_Int(115200);    //ˢ��ģ��ͨ�Žӿ�
	usart3Init();         //L620ģ��ͨ�Žӿ�
	uart4Init();
	//as608Init();  
	//Port_Init(); 
	//Key_init();	
	KEY_Init();
	fingler();
	//TIM2_Int_Init(59999,35999);
	//TIM3_Int_Init(59999,35999);
	TIM4_Int_Init(9999,7199);
	
  Uart1_SendCmdStr(pSend);
	
  Clear_Buffer();	
	L620_Init();
	
	getTime(timebuffer);       //�ӷ������õ�ʱ������
	RTC_Init(calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);  //��ʱʱ���Ѿ��浽ʱ��ṹ���У�RTCÿ�봥��һ���жϲ�����һ��ʱ��

	
	Uart3_SendCmdStr("AT+CIPSEND=7\r\n");
	delay_ms(100);
	Uart3_SendDataStr(buf1,7);   //��һ������������ӣ�ʹ�������·��û�����
	f2=getUserMessage();
	Clear_Buffer();	
	while(f2==0)
	{
		
	   Uart3_SendCmdStr("AT+CIPSEND=7\r\n");
	   delay_ms(100);
	   Uart3_SendDataStr(buf1,7);
       f2=getUserMessage();
       Clear_Buffer();	
   
	}
  delay_ms(100);
	UART1_Send_char(Usercount);
	f2=0;
		
	addr=FLASH_USERMESSAGE_ADDR1;         //��������ֵ������
	STMFLASH_Write(addr,(u16*)buf2,5);    //�洢�û��޸���Ϣ
	addr+=6;
	
	addr1=FLASH_USERMESSAGE_ADDR;         //��������ֵ������
	STMFLASH_Write(addr1,(u16*)buf3,5);   //�洢�û�������¼
	addr1+=6;	
	
	/*f4=id1016bInit();
	UART1_Send_char(f4);
	UART1_Send_char(0xFF);
	f5=delAllFinger();
	UART1_Send_char(f5);
	UART1_Send_char(0x01);
	1*/
	flag1=nfc_WakeUp();

	delay_ms(10);
	UART1_Send_char(flag1);
	 temp1=0xff;
	while(1) 
	{    	
		
		 if(Check_WKUP()==0)
		 {
			UART1_Send_char(0xAE);
			 OLED_ShowString(0,0,"Smart Lock",10); 
		     OLED_Refresh_Gram();
	
		 } 
			 
//ˢ��ƥ��	 	
		if(flag1==1)
		{
			if(nfc_detect())
			{
				  
				  for(user_num=0;user_num<Usercount;user_num++)
					{	
						 for(j=0;j<4;j++)
							 P1[j]=0;	
					   STMFLASH_Read(FLASH_SAVE_ADDR+28*user_num+1,(u16*)P1,4); //��flash��ȡ��У԰��ID
						 temp[user_num]=nfc_InListPassiveTarget(P1);
					   delay_ms(10);
					   UART1_Send_char(temp[user_num]);
					   delay_ms(15);
					   if(temp[user_num]==0x01)
					   {	
						  UART1_Send_char(0xA7);
						  temp1=0x01;
							break;
					   }
					}
					if(user_num==Usercount)
					{
						OLED_ShowString(0,36,"Unlock unsuccessful",12); 
				        OLED_Refresh_Gram();//ˢ��ʧ��
						delay_ms(1500);
						OLED_Clear();
					}
		   }
	    }
//ָ��ƥ��
		if(fingerDetect()==0x01)
		{
			UART1_Send_char(0xbc);
			if(searchFinger()==0x01)
				temp1=0x02;
			else if(searchFinger()==0x00)
			{
				OLED_ShowChinese4(0,24);
                OLED_Refresh_Gram();	
				delay_ns(1);
				OLED_Clear();
			}
		}
//����ɨ��	
			t=keyScan(0);		
		  if((ij==0)&&(t!=0xff))
		{
		   UART1_Send_char(t);
		    Keytable[ij++]=t;
			//delay_ms(50);
			t=0xff;
			UART1_Send_char(Keytable[0]);
			//delay_ms(50);
			if(ij==1)
			 {	
		          switch(Keytable[0])
				 {                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
				  case 0X0E: temp1=3;break;	 //���뿪��
				  case 0X0F: temp1=4;break;  //���ÿ���
				  default: break;
				 }
		     }
	     }
		 //ֱ��ˢ������		
			 if(temp1==1)
				{
				   temp1=0xff;
				   UART1_Send_char(0xA0);
				   OLED_ShowString(0,36,"Unlock successfully",12); 
				   OLED_Refresh_Gram();
				   delay_ms(1500);
				   OLED_Clear();
				   UART1_Send_char(0xAA);
				   
				 }  
			
 
//ֱ��ָ�ƿ���		
		
		else if(temp1==2)
			{
			   temp1=0xff;
			   OLED_ShowChinese3(0,24);
               OLED_Refresh_Gram();	
			   delay_ns(1);
			   OLED_Clear();					
			}
			   
//ֱ�����뿪��			   
			else if(temp1==3)
			   {
				   temp1=0xff;
				   ij=0;
				   Mima_kaisuo();
				  t=keyScan(0); 
				
              }
			   
			  else if(temp1==4) 
			  {  
				while(t!=0xff)  
				{
				  delay_ms(50);
				  Keytable[ij++]=t;
			      delay_ms(50);
				  UART1_Send_char(Keytable[1]);
		          delay_ms(50);
					    if(ij==2&&Keytable[0]==0x0F)
			          {	

							switch(Keytable[1])
						   {                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
							  case 0X01:  password();break;	 //������
							  case 0X02:  finger();break; 	 //���ָ��
							  case 0X03:  finger();break; 	 //ɾ��ָ�� 
							  default: break;
						   }
						      for(h=0;h<2;h++)
							  {  
							   Keytable[h]=0xff;
								 ij=0;
							  }	
		              }
					  break;
				 }		  
			  }
	
	}
    }		

	  
			   
 
		




