#include "nfc.h"
#include "delay.h"
#include "oled.h"
#include "usart3.h"
#include "stmflash.h"
#include "usart.h"
u32 Uart0_rev_countmap;//���ڶ�ȡ�����ݳ���
extern u8 RxBuffer1[500];
//extern u8 flag_1; //��һ�� ���� UID��ɹ�
//extern u8 flag_2; 
//extern u8 flag_3; 
//extern u8 flag_rev_finish;//���ڽ��յ����ݵı�־�� 1Ϊ���յ�
//extern u8 flag_nfc_status;
extern u32 RxCounter1;
//u8 UID[4]; //�洢 UID


//��USART2������ length���ȵ�����data
void SendData(u8 *data,u8 length)
{
    u8 i;
    for(i=0;i<length;i++)
    {
        USART_SendData(USART2, data[i]);
        while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)//�ȵ÷������
        {
        }  
    }
}

void CleanBuffer(u16 num)//���� ǰ ���ٸ��ֽڵ�����
{
    u16 i=0;   
    for(i=0;i<num;i++)
      USART2_RX_BUF[i]=0x00;
	  RxCount2=0;
}

////55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00  
    int nfc_WakeUp(void)
{
    u8 data[24];
    int  flag=0;
	  CleanBuffer(500);
    while(1)
    { 			  
//        flag_rev_finish=0;
        data[0]=0x55;
        data[1]=0x55;
        data[2]=0x00;
        data[3]=0x00;
        data[4]=0x00;
        data[5]=0x00;
        data[6]=0x00;
        data[7]=0x00;
        data[8]=0x00;
        data[9]=0x00;
        data[10]=0x00;
        data[11]=0x00;
        data[12]=0x00;
        data[13]=0x00;
        data[14]=0x00;
        data[15]=0x00;
        data[16]=0xFF;
        data[17]=0x03; //�� ����
        data[18]=0xFD; //�� ���� У��  0x100-data[3]
        data[19]=0xD4; //�����ʶ��
        data[20]=0x14; //�����ʶ��
        data[21]=0x01;
        data[22]=0x17; //���� У��   0x100-
        data[23]=0x00;

        SendData(data,24);//��USART1������ length���ȵ�����data
        delay_ms(180); 
       
       
   //00 00 FF 00 FF 00 00 00 FF 02 FE D5 15 16 00 
        if(USART2_RX_BUF[0]==0x00&&USART2_RX_BUF[1]==0x00&&USART2_RX_BUF[2]==0xFF&&USART2_RX_BUF[3]==0x00&&USART2_RX_BUF[4]==0xFF)
				{
					if(USART2_RX_BUF[5]==0x00&&USART2_RX_BUF[6]==0x00&&USART2_RX_BUF[7]==0x00&&USART2_RX_BUF[8]==0xFF&&USART2_RX_BUF[9]==0x02)
					{
					    if(USART2_RX_BUF[10]==0xFE&&USART2_RX_BUF[11]==0xD5&&USART2_RX_BUF[12]==0x15&&USART2_RX_BUF[13]==0x16&&USART2_RX_BUF[14]==0x00)
							{
							     flag=1;
								   CleanBuffer(500);
								   return flag;
							}
					}
				}
				CleanBuffer(500);
        delay_ms(3000);				
    }
}

//���� 106 kbps type A (ISO/IEC14443 Type A),
//00 00 FF 04 FC D4 4A 01 00 E1 00
int  nfc_InListPassiveTarget(u8 *p)
{
    u8 data[11];
	  
	  int flag=0xff;
	  CleanBuffer(500);
	  
    while(1)
    {   
        data[0]=0x00;
        data[1]=0x00;
        data[2]=0xFF;
        data[3]=0x04; //�� ����
        data[4]=0xFC; //�� ���� У��  0x100-data[3]
        data[5]=0xD4; //�����ʶ��
        data[6]=0x4A; //�����ʶ��
        data[7]=0x01;
        data[8]=0x00;
        data[9]=0xE1; //���� У��   0x100-
        data[10]=0x00;
        
        SendData(data,11);//��USART1������ length���ȵ�����data
        delay_ms(180); 
        //00 00 FF 00 FF 00 00 00 FF 0C F4 D5 4B 01 01 00 04 08 04 B0 86 4D 25 26 00
			  if(USART2_RX_BUF[19]==p[0]&&USART2_RX_BUF[20]==p[1]&&USART2_RX_BUF[21]==p[2]&&USART2_RX_BUF[22]==p[3])
				{
						flag=1;
					  CleanBuffer(500);
					  return flag;
				}
				else
				{
                  CleanBuffer(500);
//                delay_ms(3000);
                  delay_ms(200);					
					  return flag;
		    }
   }
 }

//ˢ����⺯��
 unsigned char nfc_detect(void)
 {
    u8 data[11]; 
	  int flag=0xff;
	  CleanBuffer(500);
	  while(1)
    {   
        data[0]=0x00;
        data[1]=0x00;
        data[2]=0xFF;
        data[3]=0x04; //�� ����
        data[4]=0xFC; //�� ���� У��  0x100-data[3]
        data[5]=0xD4; //�����ʶ��
        data[6]=0x4A; //�����ʶ��
        data[7]=0x01;
        data[8]=0x00;
        data[9]=0xE1; //���� У��   0x100-
        data[10]=0x00;
        
        SendData(data,11);//��USART1������ length���ȵ�����data
        delay_ms(30); 
        //00 00 FF 00 FF 00 00 00 FF 0C F4 D5 4B 01 01 00 04 08 04 B0 86 4D 25 26 00
			  if(RxCount2==25&&USART2_RX_BUF[0]==0&&USART2_RX_BUF[1]==0&&USART2_RX_BUF[2]==0xff&&USART2_RX_BUF[3]==0&&USART2_RX_BUF[4]==0xff&&USART2_RX_BUF[5]==0)
				{
						flag=1;
					  CleanBuffer(500);
				}
				else
				{
					   flag=0;
					  CleanBuffer(500);
				}
				return flag;
     }
	}

