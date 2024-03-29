#include "id1016b.h"
#include "uart4.h"
#include "usart.h"
#include "delay.h"
#include "oled.h"
#include "ctype.h"
#include "break.h"
#include "stmflash.h"
#include "rtc.h"
u8 table7[9]={0x00};
void cleanRx4Buffer()
{
    int i=0;
	  for(i=0;i<50;i++)
	      RxBuffer2[i]=0;
	  RxCounter2=0;
}
int id1016bMatch(unsigned char* s,unsigned char* d,int len)
{
    int i=0;
	  for(i=0;i<len;i++)
	  {
		    if(s[i]==d[i])
					  ;
				else
					 return 0;
		} 
	  return 1;
}
int id1016bInit()
{
	 int flag=2;
   unsigned char sendData1[26]={0x55,0xAA,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01};
   unsigned char receiveData1[26]={0xAA,0x55,0x01,0x00,0x01,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x01};
	 //UART1_Send_char(0x01);
   cleanRx4Buffer();
	 //UART1_Send_char(0x02);
	 Uart4_SendDataStr(sendData1,26);
	 //UART1_Send_char(0x03);
	 delay_ms(1000);

	 flag=id1016bMatch(RxBuffer2,receiveData1,26);
	 
	 //UART1_Send_char(0x04);
	 delay_ms(1000);
	 //UART1_Send_char(flag);
	 return flag;
}
//采集指纹图像并保存于ImageBuffer中,成功返回1，失败返回0
int getImage()
{
   int flag=2;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0x01};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x20,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x22,0x01};
   cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);
//	 for(i=0;i<26;i++)
//		   printf("%d",RxBuffer2[i]);
	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 return flag;
}
//检查收到指令时刻指纹输入状态
int fingerDetect()
{
   int flag=2;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x21,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x01};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x21,0x00,0x03,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x25,0x01};
   cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(120);
//	 for(i=0;i<26;i++)
//		   printf("%d",RxBuffer2[i]);
	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 return flag;
}
//将指纹模板保存于指定编号的指纹库中
int storeFinger(int num)
{
   int flag=2,i=0;
	 unsigned long int temp=0;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x40,0x00,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x01};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x40,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x42,0x01};
   sendData[8]=num;
	 for(i=0;i<24;i++)
		  temp+=sendData[i];
	 sendData[24]=temp&0x00ff;
	 sendData[25]=(temp&0xff00)>>8;
	 cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);
//	 for(i=0;i<26;i++)
//		   printf("%d",RxBuffer2[i]);
	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 return flag;
}
//从ImageBuffer中生成模板数据保存在RamBuffer0中
int generateFinger0()
{
   int flag=2;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x60,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x61,0x01};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x60,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x62,0x01};
   cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);
//	 for(i=0;i<26;i++)
//		   printf("%d",RxBuffer2[i]);
	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 return flag;
}
//从ImageBuffer中生成模板数据保存在RamBuffer1中
int generateFinger1()
{
   int flag=2;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x60,0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x62,0x01};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x60,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x62,0x01};
   cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);
//	 for(i=0;i<26;i++)
//		   printf("%d",RxBuffer2[i]);
	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 return flag;
}
//从ImageBuffer中生成模板数据保存在RamBuffer2中
int generateFinger2()
{
   int flag=2;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x60,0x00,0x02,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x63,0x01};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x60,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x62,0x01};
   cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);
//	 for(i=0;i<26;i++)
//		   printf("%d",RxBuffer2[i]);
	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 return flag;
}
//将指纹数据合并生成模板数据并保存于RamBuffer0中
int mergeFinger()
{
   int flag=2;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x61,0x00,0x03,0x00,0x00,0x00,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x66,0x01};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x61,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x63,0x01};
   cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);
//	 for(i=0;i<26;i++)
//		   printf("%d",RxBuffer2[i]);
	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 return flag;
}
//添加指纹，num为指纹模板保存在ImageBuffer的ID号，范围为1-99
int addFinger(int num)
{
   int flag=0;
	 u8 aa=0;
	 OLED_ShowChinese1(0,0);	 //请录入指纹		
	 OLED_Refresh_Gram();
	delay_ns(3);
	 flag=getImage();
	 UART1_Send_char(flag);
	 if(flag==1)
	 {
	    flag=0;
	    flag=generateFinger0();
	    if(flag==0)
	      return 0;
			OLED_ShowChinese11(0,24); //指纹正常
			OLED_Refresh_Gram();
		  delay_ns(2);
		  OLED_Clear();	
	      flag=0;
			OLED_ShowChinese12(0,0);  //请再按指纹
			OLED_Refresh_Gram();
		delay_ns(3);
	    flag=getImage();
	    UART1_Send_char(flag);
	    if(flag==1)
	    {
	      flag=0;
	      flag=generateFinger1();
	      if(flag==0)
		      return 0;
				OLED_ShowChinese11(0,24);  //指纹正常
				OLED_Refresh_Gram();
				delay_ns(2);
		    OLED_Clear();
			   
	      flag=0;
				OLED_ShowChinese12(0,0);  //请再按指纹
			  OLED_Refresh_Gram();
		  delay_ns(3);
	      flag=getImage();
	      UART1_Send_char(flag);
	      if(flag==1)
	      {
	        flag=0;
	        flag=generateFinger2();
	        if(flag==0)
	          return 0;
					OLED_ShowChinese11(0,24);  //指纹正常
				  OLED_Refresh_Gram();
				  delay_ns(2);
		      OLED_Clear();
			   
	        flag=0;
					OLED_ShowChinese16(0,0);    //生成模板
				  OLED_Refresh_Gram();
	        flag=mergeFinger();
	        UART1_Send_char(flag);
	        if(flag==1)
	        {
	          flag=0;
						OLED_ShowChinese17(0,24);  //模板成功
					  OLED_Refresh_Gram();
	          flag=storeFinger(num);
						if(flag==1)
						{ 
							delay_ns(2);
						  OLED_Clear();
						OLED_ShowChinese8(0,24);  //添加成功
					    OLED_Refresh_Gram();
							delay_ns(3);
					    OLED_Clear();
					    FingerTime();							
					    STMFLASH_Write(addr,(u16*)table7,9);
					    addr+=10;
					    for(aa=0;aa<9;aa++)
					      table7[aa]=0;
					    Correctnum++;
					    CorrectFlag=1;
						}
	          UART1_Send_char(flag);		
	          
	        }
			else
				return 0;
         }
	       else
		       return 0;
        }
	      else
		      return 0;
    }
	  else
	    return 0;
	return flag;
}


//删除所有指纹
int delAllFinger()
{
   int flag=2;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x44,0x00,0x04,0x00,0x01,0x00,0xF4,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3D,0x02};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x44,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x46,0x01};
   cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);
//	 for(i=0;i<26;i++)
//   printf("%d",RxBuffer2[i]);
	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 Usart1_Send(RxBuffer2,26);
	 Usart1_Send(receiveData,26);
	 return flag; 
}
//删除部分指纹
int delSomeFinger(unsigned char begin,unsigned char end)
{
   int flag=2,i=0;
	 unsigned long int temp=0;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x44,0x00,0x04,0x00,0x01,0x00,0xf4,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3D,0x02};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x44,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x46,0x01};
   cleanRx4Buffer();
	 sendData[8]=begin;
	 sendData[10]=end;
	 for(i=0;i<24;i++)
		   temp+=sendData[i];
	 sendData[24]=temp&0x00ff;
	 sendData[25]=(temp&0xff00)>>8;
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);
	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 Usart1_Send(RxBuffer2,26);
	 Usart1_Send(receiveData,26);
	 return flag;
}
//在指纹库中比对指纹
int searchFinger()
{
   int flag=0;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x63,0x00,0x06,0x00,0x00,0x00,0x01,0x00,0xF4,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x5E,0x02};
//   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x44,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x46,0x01};
   flag=getImage();
	 if(flag==0)
		   return 0;
	 flag=generateFinger0();
	 if(flag==0)
		  return 0;
	 cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);
//	 for(i=0;i<26;i++)
//		   printf("%d",RxBuffer2[i]);
//	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	  flag=0;
	 if(RxBuffer2[6]==0x05)
		   flag=1;
	 return flag;
}
//获取指定编号指纹状态（是否被存在），num为编号
//返回值为1：指定编号的指纹库已经注册，为0：指定的指纹库未被注册
int getStatus(int num)
{
   int flag=2,i=0;
	 unsigned long int temp=0;
   unsigned char sendData[26]={0x55,0xAA,0x00,0x00,0x46,0x00,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x44,0x01};
   unsigned char receiveData[26]={0xAA,0x55,0x01,0x00,0x46,0x00,0x03,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x4A,0x01};
   sendData[8]=num;
	 for(i=0;i<24;i++)
		  temp+=sendData[i];
	 sendData[24]=temp&0x00ff;
	 sendData[25]=(temp&0xff00)>>8;
	 cleanRx4Buffer();
	 Uart4_SendDataStr(sendData,26);
	 delay_ms(1000);

	 flag=id1016bMatch(RxBuffer2,receiveData,26);
	 return flag; 
}
void HexToStr(char *str,u8 *table,u8 len)  //十六进制转化为字符串
{
  char ddl,ddh;
	int i;
	for(i=0;i<len;i++)
	{
	     ddh=48+table[i]/16;
		 ddl=48+table[i]%16;
		 if(ddh>57) ddh=ddh+7;
		 if(ddl>57) ddl=ddl+7;
		 str[2*i]=ddh;
		 str[2*i+1]=ddl;
	}
	str[2*len]='\0';
}

void StrToHex(u8 *table,char *str,u8 len)
{
  char h1,h2;
	char s1,s2;
	int i;
	for(i=0;i<len;i++)
	{
	  h1=str[2*i];
		h2=str[2*i+1];
		s1=toupper(h1)-0x30;
		if(s1>9) s1-=7;
		s2=toupper(h2)-0x30;
		if(s2>9) s2-=7;
		table[i]=s1*16+s2;
	}
}



































