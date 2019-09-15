#include <string.h>
#include "delay.h" 	
#include "usart.h"
#include "as608.h"
#include "oled.h"
#include "led.h"
#include "stmflash.h"
#include "rtc.h"
#include "break.h"
u32 AS608Addr = 0XFFFFFFFF; //Ĭ��
SysPara AS608Para;//ָ��ģ��AS608����
u16 ValidN;//ģ������Чָ�Ƹ���
extern u32 addr;
extern u8 table5[9];
//��ʼ��PA6Ϊ��������		    
//��������Ӧ״̬(������Ӧʱ����ߵ�ƽ�ź�)
void PS_StaGPIO_Init(void)
{   
  GPIO_InitTypeDef  GPIO_InitStructure;
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);//ʹ��GPIOAʱ��
  //��ʼ����״̬����GPIOA
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//��������ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIO	
}
//���ڷ���һ���ֽ�
static void MYUSART_SendData(u8 data)
{
	while((USART2->SR&0X40)==0); 
	USART2->DR = data;
}
//���Ͱ�ͷ
static void SendHead(void)
{
	MYUSART_SendData(0xEF);
	MYUSART_SendData(0x01);
}
//���͵�ַ
static void SendAddr(void)
{
	MYUSART_SendData(AS608Addr>>24);
	MYUSART_SendData(AS608Addr>>16);
	MYUSART_SendData(AS608Addr>>8);
	MYUSART_SendData(AS608Addr);
}
//���Ͱ���ʶ,
static void SendFlag(u8 flag)
{
	MYUSART_SendData(flag);
}
//���Ͱ�����
static void SendLength(int length)
{
	MYUSART_SendData(length>>8);
	MYUSART_SendData(length);
}
//����ָ����
static void Sendcmd(u8 cmd)
{
	MYUSART_SendData(cmd);
}
//����У���
static void SendCheck(u16 check)
{
	MYUSART_SendData(check>>8);
	MYUSART_SendData(check);
}
//�ж��жϽ��յ�������û��Ӧ���
//waittimeΪ�ȴ��жϽ������ݵ�ʱ�䣨��λ1ms��
//����ֵ�����ݰ��׵�ַ
static u8 *JudgeStr(u16 waittime)
{
	char *data;
	u8 str[9];
	str[0]=0xef;str[1]=0x01;str[2]=AS608Addr>>24;
	str[3]=AS608Addr>>16;str[4]=AS608Addr>>8;
	str[5]=AS608Addr;str[6]=0x07;str[7]='\0';
//	str[5]=AS608Addr;str[6]=0x07;str[7]=0x03;str[8]='\0';
	USART2_RX_STA=0;
	while(--waittime)
	{
		delay_ms(1);
		if(USART2_RX_STA&0X8000)//���յ�һ������
		{
			USART2_RX_STA=0;
			data=strstr((const char*)USART2_RX_BUF,(const char*)str);
			if(data)
				return (u8*)data;	
		}
	}
	return 0;
}
//¼��ͼ�� PS_GetImage
//����:̽����ָ��̽�⵽��¼��ָ��ͼ�����ImageBuffer�� 
//ģ�鷵��ȷ����
u8 PS_GetImage(void)
{
  u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x01);
  temp =  0x01+0x03+0x01;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
	{	
		ensure=data[9];
	  UART1_Send_char(data[9]);
	}
	else
		ensure=0xff;
	return ensure;
}
//�������� PS_GenChar
//����:��ImageBuffer�е�ԭʼͼ������ָ�������ļ�����CharBuffer1��CharBuffer2			 
//����:BufferID --> charBuffer1:0x01	charBuffer1:0x02												
//ģ�鷵��ȷ����
u8 PS_GenChar(u8 BufferID)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x02);
	MYUSART_SendData(BufferID);
	temp = 0x01+0x04+0x02+BufferID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//��ȷ�ȶ���öָ������ PS_Match
//����:��ȷ�ȶ�CharBuffer1 ��CharBuffer2 �е������ļ� 
//ģ�鷵��ȷ����
u8 PS_Match(void)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x03);
	temp = 0x01+0x03+0x03;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//����ָ�� PS_Search
//����:��CharBuffer1��CharBuffer2�е������ļ����������򲿷�ָ�ƿ�.�����������򷵻�ҳ�롣			
//����:  BufferID @ref CharBuffer1	CharBuffer2
//˵��:  ģ�鷵��ȷ���֣�ҳ�루����ָ��ģ�壩
u8 PS_Search(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x04);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x04+BufferID
	+(StartPage>>8)+(u8)StartPage
	+(PageNum>>8)+(u8)PageNum;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
	{
		ensure = data[9];
		p->pageID   =(data[10]<<8)+data[11];
		p->mathscore=(data[12]<<8)+data[13];	
	}
	else
		ensure = 0xff;
	return ensure;	
}
//�ϲ�����������ģ�壩PS_RegModel
//����:��CharBuffer1��CharBuffer2�е������ļ��ϲ����� ģ��,�������CharBuffer1��CharBuffer2	
//˵��:  ģ�鷵��ȷ����
u8 PS_RegModel(void)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x05);
	temp = 0x01+0x03+0x05;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;		
}
//����ģ�� PS_StoreChar
//����:�� CharBuffer1 �� CharBuffer2 �е�ģ���ļ��浽 PageID ��flash���ݿ�λ�á�			
//����:  BufferID @ref charBuffer1:0x01	charBuffer1:0x02
//       PageID��ָ�ƿ�λ�úţ�
//˵��:  ģ�鷵��ȷ����
u8 PS_StoreChar(u8 BufferID,u16 PageID)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x06);
	Sendcmd(0x06);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	temp = 0x01+0x06+0x06+BufferID
	+(PageID>>8)+(u8)PageID;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;	
}
//ɾ��ģ�� PS_DeletChar
//����:  ɾ��flash���ݿ���ָ��ID�ſ�ʼ��N��ָ��ģ��
//����:  PageID(ָ�ƿ�ģ���)��Nɾ����ģ�������
//˵��:  ģ�鷵��ȷ����
u8 PS_DeletChar(u16 PageID,u16 N)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x0C);
	MYUSART_SendData(PageID>>8);
	MYUSART_SendData(PageID);
	MYUSART_SendData(N>>8);
	MYUSART_SendData(N);
	temp = 0x01+0x07+0x0C
	+(PageID>>8)+(u8)PageID
	+(N>>8)+(u8)N;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//���ָ�ƿ� PS_Empty
//����:  ɾ��flash���ݿ�������ָ��ģ��
//����:  ��
//˵��:  ģ�鷵��ȷ����
u8 PS_Empty(void)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0D);
	temp = 0x01+0x03+0x0D;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//дϵͳ�Ĵ��� PS_WriteReg
//����:  дģ��Ĵ���
//����:  �Ĵ������RegNum:4\5\6
//˵��:  ģ�鷵��ȷ����
u8 PS_WriteReg(u8 RegNum,u8 DATA)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x05);
	Sendcmd(0x0E);
	MYUSART_SendData(RegNum);
	MYUSART_SendData(DATA);
	temp = RegNum+DATA+0x01+0x05+0x0E;
	SendCheck(temp);
	data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	if(ensure==0)
		printf("\r\n���ò����ɹ���");
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//��ϵͳ�������� PS_ReadSysPara
//����:  ��ȡģ��Ļ��������������ʣ�����С��)
//����:  ��
//˵��:  ģ�鷵��ȷ���� + ����������16bytes��
u8 PS_ReadSysPara(SysPara *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x0F);
	temp = 0x01+0x03+0x0F;
	SendCheck(temp);
	data=JudgeStr(1000);
	if(data)
	{
		ensure = data[9];
		p->PS_max = (data[14]<<8)+data[15];
		p->PS_level = data[17];
		p->PS_addr=(data[18]<<24)+(data[19]<<16)+(data[20]<<8)+data[21];
		p->PS_size = data[23];
		p->PS_N = data[25];
	}		
	else
		ensure=0xff;
	if(ensure==0x00)
	{
		printf("\r\nģ�����ָ������=%d",p->PS_max);
		printf("\r\n�Աȵȼ�=%d",p->PS_level);
		printf("\r\n��ַ=%x",p->PS_addr);
		printf("\r\n������=%d",p->PS_N*9600);
	}
	else 
			printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//����ģ���ַ PS_SetAddr
//����:  ����ģ���ַ
//����:  PS_addr
//˵��:  ģ�鷵��ȷ����
u8 PS_SetAddr(u32 PS_addr)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x07);
	Sendcmd(0x15);
	MYUSART_SendData(PS_addr>>24);
	MYUSART_SendData(PS_addr>>16);
	MYUSART_SendData(PS_addr>>8);
	MYUSART_SendData(PS_addr);
	temp = 0x01+0x07+0x15
	+(u8)(PS_addr>>24)+(u8)(PS_addr>>16)
	+(u8)(PS_addr>>8) +(u8)PS_addr;				
	SendCheck(temp);
	AS608Addr=PS_addr;//������ָ�������ַ
  data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;	
		AS608Addr = PS_addr;
	if(ensure==0x00)
		printf("\r\n���õ�ַ�ɹ���");
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//���ܣ� ģ���ڲ�Ϊ�û�������256bytes��FLASH�ռ����ڴ��û����±�,
//	�ü��±��߼��ϱ��ֳ� 16 ��ҳ��
//����:  NotePageNum(0~15),Byte32(Ҫд�����ݣ�32���ֽ�)
//˵��:  ģ�鷵��ȷ����
u8 PS_WriteNotepad(u8 NotePageNum,u8 *Byte32)
{
	u16 temp;
  u8  ensure,i;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(36);
	Sendcmd(0x18);
	MYUSART_SendData(NotePageNum);
	for(i=0;i<32;i++)
	 {
		 MYUSART_SendData(Byte32[i]);
		 temp += Byte32[i];
	 }
  temp =0x01+36+0x18+NotePageNum+temp;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
		ensure=data[9];
	else
		ensure=0xff;
	return ensure;
}
//������PS_ReadNotepad
//���ܣ�  ��ȡFLASH�û�����128bytes����
//����:  NotePageNum(0~15)
//˵��:  ģ�鷵��ȷ����+�û���Ϣ
u8 PS_ReadNotepad(u8 NotePageNum,u8 *Byte32)
{
	u16 temp;
  u8  ensure,i;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x04);
	Sendcmd(0x19);
	MYUSART_SendData(NotePageNum);
	temp = 0x01+0x04+0x19+NotePageNum;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
	{
		ensure=data[9];
		for(i=0;i<32;i++)
		{
			Byte32[i]=data[10+i];
		}
	}
	else
		ensure=0xff;
	return ensure;
}
//��������PS_HighSpeedSearch
//���ܣ��� CharBuffer1��CharBuffer2�е������ļ��������������򲿷�ָ�ƿ⡣
//		  �����������򷵻�ҳ��,��ָ����ڵ�ȷ������ָ�ƿ��� ���ҵ�¼ʱ����
//		  �ܺõ�ָ�ƣ���ܿ�������������
//����:  BufferID�� StartPage(��ʼҳ)��PageNum��ҳ����
//˵��:  ģ�鷵��ȷ����+ҳ�루����ָ��ģ�壩
u8 PS_HighSpeedSearch(u8 BufferID,u16 StartPage,u16 PageNum,SearchResult *p)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x08);
	Sendcmd(0x1b);
	MYUSART_SendData(BufferID);
	MYUSART_SendData(StartPage>>8);
	MYUSART_SendData(StartPage);
	MYUSART_SendData(PageNum>>8);
	MYUSART_SendData(PageNum);
	temp = 0x01+0x08+0x1b+BufferID
	+(StartPage>>8)+(u8)StartPage
	+(PageNum>>8)+(u8)PageNum;
	SendCheck(temp);
	data=JudgeStr(2000);
 	if(data)
	{
		ensure=data[9];
		p->pageID 	=(data[10]<<8) +data[11];
		p->mathscore=(data[12]<<8) +data[13];
	}
	else
		ensure=0xff;
	return ensure;
}
//����Чģ����� PS_ValidTempleteNum
//���ܣ�����Чģ�����
//����: ��
//˵��: ģ�鷵��ȷ����+��Чģ�����ValidN
u8 PS_ValidTempleteNum(u16 *ValidN)
{
	u16 temp;
  u8  ensure;
	u8  *data;
	SendHead();
	SendAddr();
	SendFlag(0x01);//�������ʶ
	SendLength(0x03);
	Sendcmd(0x1d);
	temp = 0x01+0x03+0x1d;
	SendCheck(temp);
  data=JudgeStr(2000);
	if(data)
	{
		ensure=data[9];
		*ValidN = (data[10]<<8) +data[11];
	}		
	else
		ensure=0xff;
	
	if(ensure==0x00)
	{
		printf("\r\n��Чָ�Ƹ���=%d",(data[10]<<8)+data[11]);
	}
	else
		printf("\r\n%s",EnsureMessage(ensure));
	return ensure;
}
//��AS608���� PS_HandShake
//����: PS_Addr��ַָ��
//˵��: ģ�鷵�µ�ַ����ȷ��ַ��	
u8 PS_HandShake(u32 *PS_Addr)
{
	SendHead();
	SendAddr();
	MYUSART_SendData(0X01);
	MYUSART_SendData(0X00);
	MYUSART_SendData(0X00);	
	delay_ms(200);
	if(USART2_RX_STA&0X8000)//���յ�����
	{		
		if(//�ж��ǲ���ģ�鷵�ص�Ӧ���				
					USART2_RX_BUF[0]==0XEF
				&&USART2_RX_BUF[1]==0X01
				&&USART2_RX_BUF[6]==0X07
			)
			{
				*PS_Addr=(USART2_RX_BUF[2]<<24) + (USART2_RX_BUF[3]<<16)
								+(USART2_RX_BUF[4]<<8) + (USART2_RX_BUF[5]);
				USART2_RX_STA=0;
				return 0;
			}
		USART2_RX_STA=0;					
	}
	return 1;		
}
//ģ��Ӧ���ȷ������Ϣ����
//���ܣ�����ȷ���������Ϣ������Ϣ
//����: ensure
const char *EnsureMessage(u8 ensure) 
{
	const char *p;
	switch(ensure)
	{
		case  0x00:
			p="OK";break;		
		case  0x01:
			p="���ݰ����մ���";break;
		case  0x02:
			p="��������û����ָ";break;
		case  0x03:
			p="¼��ָ��ͼ��ʧ��";break;
		case  0x04:
			p="ָ��ͼ��̫�ɡ�̫��������������";break;
		case  0x05:
			p="ָ��ͼ��̫ʪ��̫��������������";break;
		case  0x06:
			p="ָ��ͼ��̫�Ҷ�����������";break;
		case  0x07:
			p="ָ��ͼ����������������̫�٣������̫С��������������";break;
		case  0x08:
			p="ָ�Ʋ�ƥ��";break;
		case  0x09:
			p="û������ָ��";break;
		case  0x0a:
			p="�����ϲ�ʧ��";break;
		case  0x0b:
			p="����ָ�ƿ�ʱ��ַ��ų���ָ�ƿⷶΧ";
		case  0x10:
			p="ɾ��ģ��ʧ��";break;
		case  0x11:
			p="���ָ�ƿ�ʧ��";break;	
		case  0x15:
			p="��������û����Чԭʼͼ��������ͼ��";break;
		case  0x18:
			p="��д FLASH ����";break;
		case  0x19:
			p="δ�������";break;
		case  0x1a:
			p="��Ч�Ĵ�����";break;
		case  0x1b:
			p="�Ĵ����趨���ݴ���";break;
		case  0x1c:
			p="���±�ҳ��ָ������";break;
		case  0x1f:
			p="ָ�ƿ���";break;
		case  0x20:
			p="��ַ����";break;
		default :
			p="ģ�鷵��ȷ��������";break;
	}
 return p;	
}
/*
//ˢָ��
void press_FR(void)
{
	SearchResult seach;
	u8 ensure;
	char *str;
	ensure=PS_GetImage();
	if(ensure==0x00)//��ȡͼ��ɹ� 
	{	
//		BEEP=1;//�򿪷�����	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //���������ɹ�
		{		
//			BEEP=0;//�رշ�����	
			ensure=PS_HighSpeedSearch(CharBuffer1,0,AS608Para.PS_max,&seach);
			if(ensure==0x00)//�����ɹ�
			{				
				LCD_Fill(0,100,lcddev.width,160,WHITE);
				Show_Str_Mid(0,100,"ˢָ�Ƴɹ�",16,240);				
				str=mymalloc(SRAMIN,50);
				sprintf(str,"ȷ�д���,ID:%d  ƥ��÷�:%d",seach.pageID,seach.mathscore);
				Show_Str_Mid(0,140,(u8*)str,16,240);
				myfree(SRAMIN,str);
			}
			else 
				ShowErrMessage(ensure);					
	  }
		else
			ShowErrMessage(ensure);
//	 BEEP=0;//�رշ�����
	 delay_ms(600);
	 LCD_Fill(0,100,lcddev.width,160,WHITE);
	}
		
}

//ɾ��ָ��
void Del_FR(void)
{
	u8  ensure;
	u16 num;
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	Show_Str_Mid(0,100,"ɾ��ָ��",16,240);
	Show_Str_Mid(0,120,"������ָ��ID��Enter����",16,240);
	Show_Str_Mid(0,140,"0=< ID <=299",16,240);
	delay_ms(50);
	AS608_load_keyboard(0,170,(u8**)kbd_delFR);
	num=GET_NUM();//��ȡ���ص���ֵ
	if(num==0xFFFF)
		goto MENU ; //������ҳ��
	else if(num==0xFF00)
		ensure=PS_Empty();//���ָ�ƿ�
	else 
		ensure=PS_DeletChar(num,1);//ɾ������ָ��
	if(ensure==0)
	{
		LCD_Fill(0,120,lcddev.width,160,WHITE);
		Show_Str_Mid(0,140,"ɾ��ָ�Ƴɹ�",16,240);		
	}
  else
		ShowErrMessage(ensure);	
	delay_ms(1200);
	PS_ValidTempleteNum(&ValidN);//����ָ�Ƹ���
	LCD_ShowNum(56,80,AS608Para.PS_max-ValidN,3,16);
MENU:	
	LCD_Fill(0,100,lcddev.width,160,WHITE);
	delay_ms(50);
	AS608_load_keyboard(0,170,(u8**)kbd_menu);
}
*/
//AS608��ʼ��
void as608Init()
{
	u8 temp;
	 temp=PS_HandShake(&AS608Addr);
	 UART1_Send_char(temp);
	 while(PS_HandShake(&AS608Addr))
	 {
//	     OLED_ShowString(0,0,(unsigned char*)" ShakHands....",16);
	 }
//	 OLED_ShowString(0,32,(unsigned char*)" Ready To Work ",12);
}

void press_FR(void)
{
	SearchResult seach;
	u8 ensure,aa;
//	OLED_ShowString(0,32,(unsigned char*)" Begin Compare ",12);
	OLED_ShowChinese2(0,0);
	ensure=PS_GetImage();
	if(ensure==0x00)// 
	{	
		ensure=PS_GenChar(CharBuffer1);
		if(ensure==0x00) //
		{		
			ensure=PS_HighSpeedSearch(CharBuffer1,0,300,&seach);
			if(ensure==0x00)//
			{		
          OLED_ShowChinese3(0,24);
          OLED_Refresh_Gram();				
//				OLED_ShowString(0,32,(unsigned char*)" Comper PASS!   ",12);
//			  OLED_ShowString(0,48,(unsigned char*)" FUN:CPR FRIGER ",12);
				  LED0=0;
		      delay_ns(5);
				  FingerTime1();	     					
					STMFLASH_Write(addr1,(u16*)table5,9);
					for(aa=0;aa<9;aa++)
					  table5[aa]=0;
					addr1+=9;
					Recordnum++;
			}
			else 
			{
				  OLED_ShowChinese4(0,24);
				  OLED_Refresh_Gram();
				  delay_ns(5);
//			  OLED_ShowString(0,32,(unsigned char*)" Comper Fail!   ",12);
//			  OLED_ShowString(0,48,(unsigned char*)" FUN:CPR FRIGER ",12);
			}				
		}
		else 
		{
			   OLED_ShowChinese4(0,24);
			   OLED_Refresh_Gram();
				 delay_ns(5);
//			 OLED_ShowString(0,32,(unsigned char*)" Comper Fail!   ",12);
//			 OLED_ShowString(0,48,(unsigned char*)" FUN:CPR FRIGER ",12);
		}		
	 delay_ms(1000);
	}
	LED0=1;
	delay_ns(3);
	OLED_Clear();
	
}
/******************************************************************************
DESC:ɾ��ָ��
*******************************************************************************/

void Del_FR(u16 ID)
{
	  u8  ensure,aa;
	  delay_ms(50);
//	  OLED_Clear();
		ensure=PS_DeletChar(ID,1);
	  //ensure=PS_Empty();
	  if(ensure==0)
	  {
			OLED_Clear();
			OLED_ShowChinese9(0,24);
//			OLED_ShowString(0,32,(unsigned char*)" FINGER DELL OK ",12);
//			OLED_ShowString(0,48,(unsigned char*)" FUN:DEL FRIGER ",12);
      OLED_Refresh_Gram();	
      FingerTime();							
			STMFLASH_Write(addr,(u16*)table5,9);
			addr+=9;
			for(aa=0;aa<9;aa++)
			  table5[aa]=0;
      Correctnum++;
      CorrectFlag=1;			
	  }
    else
		{
		  OLED_ShowChinese18(0,24);
      OLED_Refresh_Gram();	
		}			
	  delay_ms(1500);
		delay_ms(1500);
}
/***********************************************************************
DESC:���ָ��
************************************************************************/
void Add_FR(u16 ID)
{
	u8 i=0,ensure ,processnum=0,aa;
//	OLED_Clear();
	//u16 ID;
	while(1)
	{
		switch (processnum)
		{
			case 0:
				i++;
        OLED_ShowChinese1(0,0);	 //��¼��ָ��		
			  OLED_Refresh_Gram();
//			  OLED_ShowString(0,32,(unsigned char*)" Touch finger!  ",12);
//			  OLED_ShowString(0,48,(unsigned char*)" FUN:ADD FRIGER ",12);
			  UART1_Send_char(0x01);
			  UART1_Send_char(processnum);
				ensure=PS_GetImage();
			 // UART1_Send_char(ensure);
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer1); //
					if(ensure==0x00)
					{
						
//						OLED_ShowString(0,32,(unsigned char*)" Figer Correct  ",12);
//						OLED_ShowString(0,48,(unsigned char*)" FUN:ADD FRIGER ",12);
						OLED_ShowChinese11(0,24); //ָ������
						OLED_Refresh_Gram();
						delay_ns(2);
						i=0;
						processnum=1;	
            OLED_Clear();
			      delay_ns(2);
					}else ShowErrMessage(ensure);				
				}else ShowErrMessage(ensure);	
        				
				break;
			
			case 1:
				i++;
        //OLED_ShowChinese1(0,0);			
//			  OLED_ShowString(0,32,(unsigned char*)" Touch Again!   ",12);
//			  OLED_ShowString(0,48,(unsigned char*)" FUN:ADD FRIGER ",12);

			  UART1_Send_char(processnum);
			  UART1_Send_char(0x03);
			  OLED_ShowChinese12(0,0);  //���ٰ�ָ��
			  OLED_Refresh_Gram();
				ensure=PS_GetImage();
				if(ensure==0x00) 
				{
					ensure=PS_GenChar(CharBuffer2);			
					if(ensure==0x00)
					{		
            //OLED_ShowChinese1(0,0);						
//						OLED_ShowString(0,32,(unsigned char*)" Figer Correct  ",12);
//						OLED_ShowString(0,48,(unsigned char*)" FUN:ADD FRIGER ",12);
						UART1_Send_char(0x04);
						OLED_ShowChinese11(0,24);  //ָ������
						OLED_Refresh_Gram();
						i=0;
						processnum=2;
					}else ShowErrMessage(ensure);	
				}else ShowErrMessage(ensure);		
				break;

			case 2:	
//			  OLED_ShowString(0,32,(unsigned char*)" Figer Compare  ",12);
//				OLED_ShowString(0,48,(unsigned char*)" FUN:ADD FRIGER ",12);
			  OLED_Clear(); 
        delay_ns(2);
			  UART1_Send_char(processnum);
        UART1_Send_char(0x05);			
			  OLED_ShowChinese13(0,0);  //�Ա�ָ��
			  OLED_Refresh_Gram();
				ensure=PS_Match();
				if(ensure==0x00) 
				{
//					OLED_ShowString(0,32,(unsigned char*)" Figer are same ",12);
//					OLED_ShowString(0,48,(unsigned char*)" FUN:ADD FRIGER ",12);
					UART1_Send_char(0x06);
					OLED_ShowChinese14(0,24);  //�Աȳɹ�
					OLED_Refresh_Gram();
					processnum=3;
				}
				else 
				{	
          OLED_ShowChinese15(0,24);	 //�Ա�ʧ��	
          OLED_Refresh_Gram();					
					ShowErrMessage(ensure);
					i=0;
					processnum=0;		
				}
				delay_ms(1200);
				break;

			case 3:
//			  OLED_ShowString(0,32,(unsigned char*)" Touch Again!   ",12);
//			  OLED_ShowString(0,48,(unsigned char*)" FUN:ADD FRIGER ",12);
			  OLED_Clear();
			  delay_ns(2);
			  UART1_Send_char(processnum);
			  UART1_Send_char(0x07);
			  OLED_ShowChinese16(0,0);  
				OLED_Refresh_Gram();
			  ensure=PS_RegModel();
				if(ensure==0x00) 
				{
//					OLED_ShowString(0,32,(unsigned char*)" Comper Success!",12);
//			    OLED_ShowString(0,48,(unsigned char*)" FUN:ADD FRIGER ",12);
					UART1_Send_char(0x08);
					OLED_ShowChinese17(0,24);  //ģ��ɹ�
					OLED_Refresh_Gram();
					processnum=4;
				}else {processnum=0;ShowErrMessage(ensure);}
				delay_ms(1200);
				break;
				
			case 4:	
				while(!(ID<300))
        {
					 ID=1;
				}					
				ensure=PS_StoreChar(CharBuffer2,ID);
				if(ensure==0x00) 
				{
					OLED_Clear();
					delay_ns(2);
					UART1_Send_char(processnum);
					UART1_Send_char(0x09);
					OLED_ShowChinese8(0,24);  //��ӳɹ�
					OLED_Refresh_Gram();
//          OLED_ShowString(0,32,(unsigned char*)"  ADD Success!  ",12);
//			    OLED_ShowString(0,48,(unsigned char*)" FUN:ADD FRIGER ",12);					
					//PS_ValidTempleteNum(&ValidN);
					delay_ns(3);
					OLED_Clear();
					 FingerTime();							
					 STMFLASH_Write(addr,(u16*)table5,9);
					 addr+=9;
					 for(aa=0;aa<9;aa++)
					   table5[aa]=0;
					 Correctnum++;
					 CorrectFlag=1;
					return ;
				}else {processnum=0;ShowErrMessage(ensure);}					
				break;				
		}
		delay_ms(500);
		if(i==10)//
		{	
			break;	
		}				
	}
}
/*************************************************************************
 DESC:��ʾ������Ϣ
************************************************************************/
void ShowErrMessage(u8 ensure)
{
  if(ensure!=0)
	{
	   
	}
}
