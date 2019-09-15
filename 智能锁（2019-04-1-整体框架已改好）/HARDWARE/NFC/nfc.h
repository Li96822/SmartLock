#ifndef __NFC_H
#define __NFC_H
#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "oled.h"
#include "usart3.h"
int nfc_InListPassiveTarget(u8 *p);   //配置 106 kbps type A (ISO/IEC14443 Type A),

//往USART1，发送 length长度的数据data
void SendData(u8 *data,u8 length);

int nfc_WakeUp(void);//唤醒

void CleanBuffer(u16 num);//清零 前 多少个字节的数据
unsigned char nfc_detect(void);
#endif

