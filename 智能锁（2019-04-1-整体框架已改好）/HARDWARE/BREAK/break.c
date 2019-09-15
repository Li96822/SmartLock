#include "break.h"
#include "delay.h"
#include "key.h"
#include "stmflash.h"
#include "oled.h"
#include "string.h"
#include "led.h"
#include "usart.h"
#include "nfc.h"
#include "usart3.h"
#include "as608.h"
#include "rtc.h"
#include "L620.h"
#include "id1016b.h"
#include "sys.h"
//#include "button.h"
//#define I_Q PAout(6)
extern unsigned char r1[2];//OK
extern unsigned char buf4[];
//extern u8 ij;

u8 tempdata[11];
 u8 table1[6];
u8 table2[6];
//u8 table3[11]={0x00};
//u8 table4[6]={0x00};
u8 table5[9]={0x00};
u8 table6[9]={0x00};
u8 Keytable[2]={0xff,0xff}; 

u8 Flag=0,temp2;
u32 addr;  //用户修改信息起始地址变量
u32 addr1; //用户开锁记录起始地址变量
u8 Correctnum=0;   //信息修改次数   
u8 Recordnum=0;    //用户开锁次数
u8 CorrectFlag=0;  //用户信息更新状态发送标志
u8 RecordFlag=0;    //用户开锁信息发送标志
unsigned char Match(unsigned char a,unsigned char b)  //用户密码匹配函数
{
 	u8 i=0,k=0,j=0;
	for(i=0;i<a;i++)
	{ 
		
	  j=0;
		k=i;
		
		while((j<b)&&(k<a))
		{ 
      
			if(table3[k]==table4[j])
			{
			  k++;
			  j++;
			}
      else			
			  break;
			
			
		}
		if(j==b)
			return 1;
	}
	return 0;
}
unsigned char MATCH(void)
{
	u8 a=sizeof(table3),b=sizeof(table4);
	return Match(a,b);	
}
void Permission(void)     //获取权限
{

//	u8 flag=0;
	u8 P1[8];
	u8 i=0,j=0;
	u8 temp1;
  while(i<Usercount)
  { 
	   UART1_Send_char(0x08);
	//	 flag=nfc_WakeUp();
		 if(nfc_detect())
		 {
		   STMFLASH_Read(FLASH_SAVE_ADDR+28*i+1,(u16*)P1,4); //从flash中取出校园卡ID
		   delay_ms(200);
		   temp1=nfc_InListPassiveTarget(P1);
		   for(j=0;j<4;j++)
		     P1[j]=0;
			  if(temp1==1)
			  {
			     Flag=1;
		       OLED_ShowString(0,36,"Swipe successfully",12); 
    	     OLED_Refresh_Gram();
				   delay_ns(2);
				   temp2=i;             //用户名为temp2+1
				   break;
			  }
			  i++;
	   }
  }
	OLED_Clear();
}
void Mima_kaisuo(void)
{
	u8 i=0;
	int temp=0;
	u8 flag1=0;
	u8 mm,kk,bb,aa;
	OLED_ShowString(0,24,"input password:",12);  //请输入密码
	OLED_Refresh_Gram();		   //更新显示到OLED
	while(1)
	{
		temp=keyScan(0);
		delay_ms(50);
		if(temp!=0xff)      // 说明有按键按下
		{
			if(i<10&&temp!=0x0F)     //长度小于10个，且没有按下#键
				{
					UART1_Send_char(0x02);
					tempdata[i]=temp;
					UART1_Send_char(temp);
					OLED_ShowChar(90+i*6,24,'*',12,1);
					OLED_Refresh_Gram();
					i++;
				}
			else if(temp==0X0F)   //长度小于等于10，且按下#号键
				{
					tempdata[i]=temp;
					break;
				}
				
		}
	}
	 Usart1_Send(tempdata,i+1);
	for(kk=0;kk<i+1;kk++)
	table3[kk]=tempdata[kk];
	for(bb=0;bb<Usercount;bb++)   ////依次读出flash中用户密码
	 {
	    STMFLASH_Read(FLASH_SAVE_ADDR+14+UserName[bb]*28,(u16*)table1,6);
		 	if((i<11)&&(tempdata[i]==0x0f))
	      {
				  for(mm=0;mm<6;mm++)
				    table4[mm]=table1[mm];
				  if(MATCH())   flag1=1;
				  if(flag1==1)
					   {
		        flag1=0;
		        UART1_Send_char(0x04);
		        OLED_ShowString(0,36,"Password is True",12);
	          OLED_Refresh_Gram();		//正确，显示密码正确，点亮LED
						RecordFlag=1;
		        LED0=0;
		        delay_ns(3);
					  PasswordTime1();	     					
					  STMFLASH_Write(addr1,(u16*)table5,9);
					  for(aa=0;aa<9;aa++)
					    table5[aa]=0;
					  addr1+=10;
					  Recordnum++;
					  break;
	        }		
       }
				
		}
	 if(flag1==0&&bb==Usercount)
	   {
		     UART1_Send_char(0x05);
		     OLED_ShowString(0,36,"Password is Wrong",12);//错误，显示密码错误
	       OLED_Refresh_Gram();
				 delay_ns(3);
     }
	   OLED_Clear();
		// break;	
}
void password(void)
{ 
	u8 i=0;
	int temp=0;
//	u8 flag1=0;
//  u8 mm,kk,nn,qq,aa,bb;
	u8 aa,qq,nn;
	OLED_ShowString(0,0,"Smart Lock",24); 
	OLED_Refresh_Gram();		   //更新显示到OLED
	while(1)           //等待按键按下
	{
/*		if(Keytable[0]==0x0E)      //按下#号键，表示要输入密码
		{
			OLED_ShowString(0,24,"input password:",12);  //请输入密码
	    OLED_Refresh_Gram();		   //更新显示到OLED
			while(1)
			{  	  
				temp=keyScan(0);
				delay_ms(500);
				if(temp!=0xff)      // 说明有按键按下
				{
					if(i<10&&temp!=0x0F)     //长度小于10个，且没有按下#键
					{
						UART1_Send_char(0x02);
						tempdata[i]=temp;
						UART1_Send_char(temp);
						OLED_ShowChar(90+i*6,24,'*',12,1);
						OLED_Refresh_Gram();
						i++;
					}
					else if(temp==0X0F)   //长度小于等于10，且按下#号键
					{
						tempdata[i]=temp;
						break;
					}
				
					else 
					{ 
					  UART1_Send_char(0x03);
						OLED_ShowString(0,36,"Password is too long",12);//提示密码长度过长
						OLED_Refresh_Gram();
						delay_ns(5);
						break;
					}			
					
				 }
			}
		
	    Usart1_Send(tempdata,i+1);
			for(kk=0;kk<i+1;kk++)
			  table3[kk]=tempdata[kk];
			for(bb=0;bb<Usercount;bb++)   ////依次读出flash中用户密码
			{
	      STMFLASH_Read(FLASH_SAVE_ADDR+14+UserName[bb]*28,(u16*)table1,6); 
				if((i<11)&&(tempdata[i]==0x0f))
	      {
				  for(mm=0;mm<6;mm++)
				    table4[mm]=table1[mm];
				  if(MATCH())   flag1=1;
				  if(flag1==1)
	        {
		        flag1=0;
		        UART1_Send_char(0x04);
		        OLED_ShowString(0,36,"Password is True",12);
	          OLED_Refresh_Gram();		//正确，显示密码正确，点亮LED
						RecordFlag=1;
		        LED0=0;
		        delay_ns(5);
					  PasswordTime1();	     					
					  STMFLASH_Write(addr1,(u16*)table5,9);
					  for(aa=0;aa<9;aa++)
					    table5[aa]=0;
					  addr1+=10;
					  Recordnum++;
					  break;
	        }		
       }
				
		}
		 if(flag1==0&&bb==Usercount)
	   {
		     UART1_Send_char(0x05);
		     OLED_ShowString(0,36,"Password is Wrong",12);//错误，显示密码错误
	       OLED_Refresh_Gram();
				 delay_ns(5);
     }
		 
	   LED0=1;
     OLED_Clear();	
	   //STMFLASH_Read(FLASH_SAVE_ADDR+2,(u16*)table2,6);//读flash匹配密码
		 Usart1_Send(table1,6);
		 Usart1_Send(table2,6);
		 OLED_Clear();
		 break;	
    }*/
		if(Keytable[1]==0x01)      //按下*号键表明要修改密码
		{
			OLED_ShowString(0,24,"Please swipe:",12);
			OLED_Refresh_Gram();
			delay_ms(200);
			Permission();	
			delay_ms(500);
			OLED_Clear();
			OLED_ShowString(0,0,"Smart Lock",24); 
	    OLED_Refresh_Gram();		  
			if(Flag==1)  
			{
			OLED_ShowString(0,24,"Input New PWD:",12);  //请输入新密码
	    OLED_Refresh_Gram();		                //更新显示到OLED
			while(1)
			{
			  temp=keyScan(0);
				delay_ms(500);
				if(temp!=0xff)                        //有按键按下
				{
				  if(temp>=0&&temp<=9&&i<6)      //按下的数字在1-9之间
					{    
               UART1_Send_char(0x02);						
							 OLED_ShowChar(84+i*6,24,'*',12,1);
						   OLED_Refresh_Gram();
						   tempdata[i]=temp;
						   UART1_Send_char(temp);
							 i++;
					}
					else if(i==6&&temp==0x0F)      //输满6位密码且以*结尾,将tempdata内值更新到FLASH
					{ 
						Flag=0;
						UART1_Send_char(0x03);	
						STMFLASH_Write(FLASH_SAVE_ADDR+temp2*28+14,(u16*)tempdata,6);  //将密码信息存入FLASH
						STMFLASH_Read(FLASH_SAVE_ADDR+temp2*28+14,(u16*)table1,6);      //从FLASH中读取保存的密码
						UART1_Send_char(temp2);
						Usart1_Send(tempdata,6);
						Usart1_Send(table1,6);
						for(nn=0;nn<6;nn++)
						  table3[nn]=tempdata[nn];
						for(qq=0;qq<6;qq++)
						  table4[qq]=table1[qq];
						if(MATCH())                          //存入成功
						{
							UART1_Send_char(0x04);	
							OLED_ShowString(0,36,"PWD changed OK",12); 
							OLED_Refresh_Gram();
							
              PasswordTime();							
							STMFLASH_Write(addr,(u16*)table5,9);
							for(aa=0;aa<9;aa++)
					      table5[aa]=0;
							addr+=10;
							Correctnum++;
							CorrectFlag=1;
						}
						break;
					}
					else
					{ 
						UART1_Send_char(0x05);	
						OLED_ShowString(0,36,"PWD Form is FLASE",12);       //输入的密码中有1-9之外的数
	          OLED_Refresh_Gram();	//显示密码格式错误
						break;
						
					}
					
				}
				
			}
		  delay_ms(1500);
		  OLED_Clear();	
	  	break;	
		}
//		else if(Flag!=1)
//		{
//			OLED_ShowString(0,24,"You need swipe!",12);  
//	    OLED_Refresh_Gram();		                //更新显示到OLED
//		  delay_ms(1500);	
//		  break;
//		}
	}
 }
}

void shuaka(void)
{
  u8 flag=0;
	u8 P1[8];
	u8 temp1,aa;
	u8 j=0,i=0;
	OLED_ShowString(0,0,"Smart Lock",24); 
	OLED_ShowString(0,24,"Please swipe:",12);
	OLED_Refresh_Gram();
	while(i<Usercount)
  { 
		 flag=nfc_WakeUp();

	   if(flag==1)
	   {

		    //STMFLASH_Read(FLASH_SAVE_ADDR+1,(u16*)P1,4); //从flash中读取校园卡ID
		    STMFLASH_Read(FLASH_SAVE_ADDR+28*i+1,(u16*)P1,4); //从flash中取出校园卡ID
		    
		   delay_ns(6);
		    temp1=nfc_InListPassiveTarget(P1);
		    for(j=0;j<4;j++)
		      P1[j]=0;			  
			  delay_ms(1500);
			  delay_ms(1500);
			  delay_ms(1500);
			  if(temp1==1)
			  {
					 
				   OLED_ShowString(0,36,"Unlock successfully",12); 
    	     OLED_Refresh_Gram();
					 RecordFlag=1;
				   LED0=0;
		       delay_ns(5);
				   LED0=1;
				   OLED_Clear();
				  ShuakaTime();	     					
				  STMFLASH_Write(addr1,(u16*)table5,9);
				  for(aa=0;aa<9;aa++)
					   table5[aa]=0;
				  addr1+=10;
				  Recordnum++;
					break;
			  }
        i++;				
	  }
	  OLED_Clear();
  }
}

void finger(void)
{ 
  u8 flag=0,aa=0;
	u8 bb=0;
//	u8 h;
  OLED_ShowString(0,0,"Smart Lock",24); 
	OLED_Refresh_Gram();
  while(1) 
	{		//比对指纹
    //if(KEY2==0)
		
				
//		if(I_Q)
//		{	
/*		
      flag=searchFinger();
               OLED_Clear();				 
		       delay_ms(1000);
			   OLED_ShowChinese2(0,0);
			   OLED_Refresh_Gram();
				
				if(flag==1)
				{
					flag=0;
				  OLED_ShowChinese3(0,24);
                  OLED_Refresh_Gram();	
					RecordFlag=1;
				  LED0=0;
		         delay_ns(5);
				  FingerTime1();	     					
					STMFLASH_Write(addr1,(u16*)table5,9);
					for(aa=0;aa<9;aa++)
					  table5[aa]=0;
					addr1+=10;
					Recordnum++;
					break;
				}
				else
				{
				  OLED_ShowChinese4(0,24);
				  OLED_Refresh_Gram();
				  delay_ns(5);
				}
				LED0=1;
	      delay_ns(3);
	      OLED_Clear();
		
*/			
//		}				 
	//添加指纹
	  if(Keytable[1]==0x02)
		{	
			OLED_ShowString(0,24,"Please swipe:",12);  //请刷卡
			OLED_Refresh_Gram();	
			delay_ns(2);
			Permission();
      while(Flag==0);			
      if(Flag==1)               //swiped card
      {	
        Flag=0;				
        OLED_Clear();				 
		    delay_ms(10);
		  OLED_ShowString(0,24,"You need figer!",12);	
	                OLED_Refresh_Gram();		                //更新显示到OLED
				   delay_ns(2);	
/*		  
			  while(fingerDetect())
			  {
				  OLED_ShowString(0,24,"You need figer!",12);	
	                OLED_Refresh_Gram();		                //更新显示到OLED
				   delay_ms(10);	
				  UART1_Send_char(0xA2);
		      }
*/
			    if(fingerDetect()==0x01)
				  { UART1_Send_char(0xA7);
					  flag=addFinger(temp2+1);
					
					 if(flag==1)
					 {
					   flag=0;
						UART1_Send_char(0xA8);	
//						for(h=0;h<2;h++)
//							  {  
//							   Keytable[h]=0xff;
//								
//							  }	 	
				//	OLED_ShowChinese8(0,24);	 
					break;	 
					 }
					 else
					 {
					   OLED_ShowChinese7(0,24);
					//	OLED_ShowChinese8(0,24); 
						 delay_ms(1000);
					   OLED_Clear();
				   break;
					 }
			  }	
			delay_ms(500);	
			OLED_Clear(); 
			  
		} 
	  
	}	
		
		//删除指纹
	  if(Keytable[1]==0x03)
		{
			 OLED_ShowString(0,24,"Please swipe:",12);  //请刷卡
			 OLED_Refresh_Gram();	
			 delay_ns(2);
			 Permission();
       while(Flag==0);			
       if(Flag==1) 
       {	
             Flag=0;				 
         	 OLED_ShowChinese1(0,0);	 //请录入指纹		
	         OLED_Refresh_Gram();	 
		     delay_ns(3);
			   if(fingerDetect()==0x01)
			   {
				   delay_ns(3);
			      //Del_FR(temp2+1);
//            flag=delSomeFinger((temp2+1),(temp2+1));
					  bb=getStatus(temp2+1);
					  if(bb==1)
						{
								flag=delSomeFinger((temp2+1),(temp2+1));
								if(flag==1)
								{
									flag=0;
									OLED_ShowChinese9(0,24);
									OLED_Refresh_Gram();	
									FingerTime();							
									STMFLASH_Write(addr,(u16*)table5,9);
									addr+=10;
									for(aa=0;aa<9;aa++)
										 table5[aa]=0;
									Correctnum++;
									CorrectFlag=1;
									break;							
								}
								else
								{
									OLED_ShowChinese18(0,24);
									OLED_Refresh_Gram();	
								}
					  }
            else 
            {
						     OLED_ShowChinese19(0,24);  
							 OLED_Refresh_Gram();		                //更新显示到OLED
							 delay_ns(1);
							 OLED_Clear();
							   break;
						}							
			   }

			
		}
      else if(Flag!=1)
			 {
				OLED_ShowString(0,24,"You need swipe!",12);  
	      OLED_Refresh_Gram();		                //更新显示到OLED
		    delay_ms(1500);	
			 }
			 delay_ns(3);	
			 OLED_Clear(); 			 
	}	  	
 }
}
void remote(void)
{ 
	unsigned char flag;
	 OLED_ShowString(0,0,"Smart Lock",24); 
	 OLED_Refresh_Gram();		//更新显示到OLED
   while(flag==0)		//唤醒L620NB模块
			{
					Uart3_SendCmdStr("AT\r\n"); 
					delay_ms(1000);
					flag=L620Match(RxBuffer,r1,2);
					Clear_Buffer();	
			}
	 UART1_Send_char(0x01);//唤醒标志
	 Uart3_SendCmdStr("AT+CIPSEND=5\r\n");
	 delay_ms(1500);	
	 Uart3_SendDataStr(buf4,5);
	 TIM_Cmd(TIM4, ENABLE);		
	
}


