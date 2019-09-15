#include "stm32f10x.h"
#include "L620.h"
#include "sys.h"
#include "delay.h"
#include "Usart3.h"
#include "usart.h"
#include "stmflash.h"
#include "rtc.h"

u8 Usercount=0;  //记录用户数
u8 UserName[8]={0x00};  //存储用户名
u16 year;
u8 month,date,hour,min,sec;
extern unsigned char len;
unsigned char r1[2]={0x4f,0x4b};//OK
unsigned char r2[10]={0x43,0x4f,0x4e,0x4e,0x45,0x43,0x54,0x20,0x4f,0x4b};//CONNECT OK
unsigned char r3[5]={0x1f,0x3d,0xee,0xff,0xff};
unsigned char r4[3]={0x1f,0x3d,0x11};
unsigned char time1[5]={0x1f,0x3d,0x11,0xff,0xff};
unsigned char card[5]={0x00};
unsigned char card1[6]={0xFF,0XFF,0XFF,0XFF,0XFF,0XFF};
unsigned char idcard[8]={0x00};
unsigned char dgcard[6]={0x01,0x01,0x01,0x01,0x01,0x01};
unsigned char IPtable[48]={0x00};
unsigned char Erase[27]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//char pSend[]="AT+CIPSTART=\"TCP\",\"106.14.168.235\",9999\r\n";
char pSend[48]="AT+CIPSTART=\"TCP\",\"180.153.43.173\",7012\r\n";
//char pSend[48]="AT+CIPSTART=\"TCP\",\"180.153.41.76\",7002\r\n";

void Clear_Buffer()//清空缓存
{
	  unsigned int i=0;	  
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);  
		RxCounter=0;  
	  for(i=0;i<600;i++)
	     RxBuffer[i]=0;
}
unsigned char L620Match(unsigned char *s,unsigned char *d,unsigned char len)
{
    int i=0,j=0;
    for(i=0;i<600;i++)
    {
		     if(s[i]==d[0])
				 {
				     for(j=0;j<len;j++)
					   {
						     if(s[i+j]==d[j])
									   ;
								 else
									   goto flag1;
						 }
						 return 1;
				 }
				 flag1:    ;
		}
    return 0;		
}
void L620_Init(void)
{
	 unsigned int cont;
	 unsigned char flag=0;
	
	 GPIO_ResetBits(GPIOE,GPIO_Pin_13);           //PE13 输出低 powerkey
	 for(cont=0;cont<1;cont++)
	 {
	   delay_ms(500);
	 }
	 GPIO_SetBits(GPIOE,GPIO_Pin_13);						 //PE13 输出高
	
	 GPIO_ResetBits(GPIOE,GPIO_Pin_14);           //PE14 输出低 RSTKEY
	 for(cont=0;cont<1;cont++)
	 {
	   delay_ms(500);
	 }
	 GPIO_SetBits(GPIOE,GPIO_Pin_14);						 //PE14 输出高
	 
	 
	 	UART1_Send_char(0x01);
    Uart3_SendCmdStr("AT\r\n"); 
		delay_ms(1000);
	  flag=L620Match(RxBuffer,r1,2);
		Clear_Buffer();	
		while(flag==0)
		{
				
				Uart3_SendCmdStr("AT\r\n"); 
				delay_ms(1000);
			  flag=L620Match(RxBuffer,r1,2);
			  Clear_Buffer();	
			
		}
		UART1_Send_char(0x02);
		
		Clear_Buffer();
		delay_ms(1000);
		flag=0;
		Uart3_SendCmdStr("AT+CGREG?\r\n"); 
		delay_ms(1000);
		flag=L620Match(RxBuffer,r1,2);
		Clear_Buffer();	
		while(flag==0)
		{
				Clear_Buffer();	
				Uart3_SendCmdStr("AT+CGREG?\r\n"); 
				delay_ms(1000);
			  flag=L620Match(RxBuffer,r1,2);
		}
		UART1_Send_char(0x03);
		
		Clear_Buffer();
		delay_ms(1000);
		flag=0;
		Uart3_SendCmdStr("AT+CIPMUX=0\r\n"); 
		delay_ms(1000);
		flag=L620Match(RxBuffer,r1,2);
		Clear_Buffer();	
		while(flag==0)
		{
				Clear_Buffer();	
				Uart3_SendCmdStr("AT+CIPMUX=0\r\n"); 
				delay_ms(1000);
			  flag=L620Match(RxBuffer,r1,2);
		}
		UART1_Send_char(0x04);
		
		Clear_Buffer();
		delay_ms(1000);
		flag=0;
		//Uart3_SendCmdStr(IPtable); 
		//Uart3_SendDataStr(IPtable,len);
		Uart3_SendCmdStr(pSend);
		delay_ms(1000);
		flag=L620Match(RxBuffer,r2,10);
		//Usart1_Send(RxBuffer,10);
		Clear_Buffer();	
		while(flag==0)
		{
				Clear_Buffer();	
				Uart3_SendCmdStr(pSend);
			 // Uart3_SendDataStr(IPtable,len);
				delay_ms(1000);
			    flag=L620Match(RxBuffer,r2,10);
			
		}
		Clear_Buffer();
		UART1_Send_char(0x05);

}

void strcpymessage(unsigned char *s,unsigned char *des,unsigned char i,unsigned char len)
{
    int j=0;
	  for(j=0;j<len;j++)
	  {
		    des[j]=s[i+j];
		}
}
void getTime(unsigned char *timebuffer)
{
    unsigned char contn=0;
	  int i=0,j=0;
	  Clear_Buffer();

  	Uart3_SendCmdStr("AT+CIPSEND=5\r\n");
    delay_ms(1000);
    Uart3_SendDataStr(time1,5);
    delay_ms(1000);  
	  for(contn=0;contn<5;contn++)
	  {
	    delay_ms(1000); 
	  }
    for(i=0;i<600;i++)
    {
		     if((RxBuffer[i]==0x1f)&&(RxBuffer[i+1]==0x3d))
				 {
					   if(RxBuffer[i+2]==0x11)
				     {
						     if((RxBuffer[i+10]==0xff)&&(RxBuffer[i+11]==0xff))
                 {
								
									   for(j=0;j<7;j++)
									       timebuffer[j]=0;
								     strcpymessage(RxBuffer,timebuffer,i+3,7);
                     calendar.w_year=(timebuffer[0]<<8)+timebuffer[1];
                     calendar.w_month=timebuffer[2];
                     calendar.w_date=timebuffer[3];
                     calendar.hour=timebuffer[4];
                     calendar.min=timebuffer[5];
                     calendar.sec=timebuffer[6];		

											UART1_Send_char(calendar.w_year>>8);
											UART1_Send_char(calendar.w_year);
											UART1_Send_char(calendar.w_month);
											UART1_Send_char(calendar.w_date);
											UART1_Send_char(calendar.hour); 
											UART1_Send_char(calendar.min);
											UART1_Send_char(calendar.sec);

									 	 
								 }									 
						 }
				 }
		}
	
    //STMFLASH_Write(FLASH_SAVE_ADDR,(u16 *)timebuffer,6);
		Clear_Buffer();
}
//告知L620收到了数据
unsigned char receivedData()
{
	  unsigned char flag=0;
    flag=L620Match(RxBuffer,r3,5);
	  return flag;	
}
//服务器下发用户信息数据
unsigned char getUserMessage()
{
	  int i=0;
	  Usercount=0;
	  for(i=0;i<600;i++)
	  {
		   if((RxBuffer[i]==0x1f)&&(RxBuffer[i+1]==0x3d)&&(RxBuffer[i+2]==0x01))
       {
			     i=i+3;//i=i+3;
				   while((RxBuffer[i]!=0xff)&&(RxBuffer[i+1]!=0xff))
           {  
						  //UART1_Send_char(0X06);
						 
						  Usercount++;
					    strcpymessage(RxBuffer,card,i,5);      //用户名没有存
						 // Usart1_Send(card,5);
              strcpymessage(RxBuffer,idcard,i+5,8);
						//  Usart1_Send(idcard,8);
						  //STMFLASH_Write(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*27,(u16*)card1,6);	   
						 
              STMFLASH_Write(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*28,(u16*)card,5);	   //存储用户名和校园卡ID		
						 // STMFLASH_Read(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*27,(u16*)card,5);
						 // Usart1_Send(card,5);
						 
              STMFLASH_Write(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*28+6,(u16*)idcard,8); //存储身份证ID
						 // STMFLASH_Read(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*27+6,(u16*)idcard,8);
						 // Usart1_Send(idcard,8);
						 
						  STMFLASH_Write(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*28+14,(u16*)dgcard,6);//存储初始密码
						 // STMFLASH_Read(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*27+14,(u16*)dgcard,6);
						 // Usart1_Send(dgcard,6);
						 
						  STMFLASH_Read(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*28,(u16*)card,5);
						  Usart1_Send(card,5);
						  STMFLASH_Read(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*28+6,(u16*)idcard,8);
						  Usart1_Send(idcard,8);
						  STMFLASH_Read(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*28+14,(u16*)dgcard,6);
						  Usart1_Send(dgcard,6);
						 
						 
						 
              i=i+13;						 
					 }	
           return 1;						 
			 }				 
		}
		return 0;
}

unsigned char delUserMessage()
{
	  int i=0;
	  for(i=0;i<600;i++)
	  {
		    if((RxBuffer[i]==0x1f)&&(RxBuffer[i+1]==0x3d)&&(RxBuffer[i+2]==0x02))
				{
				   i+=3;
					 while((RxBuffer[i]!=0xff)&&(RxBuffer[i]!=0x24))  //0x24为'$'
           {
					    Usercount--;                                                            //用户数减一
              STMFLASH_Write(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*27,(u16*)Erase,27);	   //往对应用户的存储区写入0xff，表示擦除用户信息				     
              i=i+1;  						 
					 }	
					 if(RxBuffer[i]==0xff)	 
           {
						 return 1;
						 
					 }
					 else if(RxBuffer[i]==0x24)
					 {
						 Usercount=0;
					   i+=2;
						 while((RxBuffer[i]!=0xff)&&(RxBuffer[i+1]!=0xff))
						 {
						    Usercount++;
							  UserName[Usercount-1]=RxBuffer[i];              //获取用户名
							  strcpymessage(RxBuffer,card,i,5);      
                strcpymessage(RxBuffer,idcard,i+5,8);
                STMFLASH_Write(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*27,(u16*)card,5);	   //存储用户名和校园卡ID					 
                STMFLASH_Write(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*27+5,(u16*)idcard,8); //存储身份证ID
						    STMFLASH_Write(FLASH_SAVE_ADDR+(RxBuffer[i]-1)*27+13,(u16*)dgcard,6);//存储初始密码
                i=i+13;						 
						 }
						 return 1;
					 }
				}
				
		}
    return 0;
}

unsigned char IPMessage(void)
{
  unsigned char i;
	if(RxCount1<=48)
	{
		//UART1_Send_char(0xCD);//&&(USART_RX_BUF[2]=='+')&&(USART_RX_BUF[3]=='C')&&(USART_RX_BUF[4]=='I')&&(USART_RX_BUF[5]=='P')&&(USART_RX_BUF[6]=='S')&&(USART_RX_BUF[7]=='T')&&(USART_RX_BUF[8]=='A')&&(USART_RX_BUF[9]=='R')&&(USART_RX_BUF[10]=='T')
    if((USART_RX_BUF[0]=='A')&&(USART_RX_BUF[1]=='T')&&(USART_RX_BUF[2]=='+')&&(USART_RX_BUF[3]=='C')&&(USART_RX_BUF[4]=='I')&&(USART_RX_BUF[5]=='P')&&(USART_RX_BUF[6]=='S')&&(USART_RX_BUF[7]=='T')&&(USART_RX_BUF[8]=='A')&&(USART_RX_BUF[9]=='R')&&(USART_RX_BUF[10]=='T'))
	  {  
			 //UART1_Send_char('A');
			 Usart1_Send(USART_RX_BUF,48);
	     STMFLASH_Write(FLASH_IP_ADDR,(u16 *)USART_RX_BUF,48);			
			 for(i=0;i<48;i++)
         USART_RX_BUF[i]=0; 
	     RxCount1=0;
			 return 1;
 	  }  
	  //UART1_Send_char(0x03);
  }
	
	//UART1_Send_char(0x04);
	return 0;
}

unsigned char Length(unsigned char *P)
{
  unsigned char length=1;
	while(*P!='n')
	{
	   P++;
		 length++;
	}
	return length;
}







































