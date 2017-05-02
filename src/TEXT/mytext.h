#ifndef __TEXT_H__
#define __TEXT_H__	 

#include <stm32f4xx.h>
#include "fontupd.h"
#include <stdio.h>
#include <stdarg.h>

#define USE_SD_FONT		(1)

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//������ʾ ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/15
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

//LCD_printf�����õ��Ľṹ
typedef struct
{
	u8  mode;  		//ģʽ
	u8	size;		//�����С
	u16	maxwidth;	//��ʾ�����
}_Typedef_lcdprintf;

extern _Typedef_lcdprintf lcd_printf;
 					     
void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);			//�õ����ֵĵ�����
void Show_Font(u16 x,u16 y,u8 *font,u8 size,u8 mode);					//��ָ��λ����ʾһ������
void Show_Str(u16 x,u16 y,u16 width,u16 height,u8*str,u8 size,u8 mode);	//��ָ��λ����ʾһ���ַ��� 
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);
void LCD_printf(u16 x, u16 y, const char *format, ...);

#endif
