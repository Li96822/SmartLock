#ifndef __NFC_H
#define __NFC_H
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "oled.h"
#include "usart3.h"
int nfc_InListPassiveTarget(u8 *p);   //���� 106 kbps type A (ISO/IEC14443 Type A),

//��USART1������ length���ȵ�����data
void SendData(u8 *data,u8 length);

int nfc_WakeUp(void);//����

void CleanBuffer(u16 num);//���� ǰ ���ٸ��ֽڵ�����
unsigned char nfc_detect(void);
#endif

