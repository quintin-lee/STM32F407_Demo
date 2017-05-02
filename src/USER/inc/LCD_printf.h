/************************************************************************
 * @file	: LCD_printf.H
 * @by		:
 * @version	: V1.0
 * @Date    : 2014 / 10 / 5
 *************************************************************************
 */
#ifndef __LCD_PRINTF_H
#define __LCD_PRINTF_H

#include "sys.h"
#include "bsp_lcd.h"

//����_LCD_Support_GBK ���Ϊ1,��֧������,����֧��
#define  _LCD_Support_GBK	1

typedef struct
{
	_lcd_dev _lcdobj;
	
#if ((_LCD_Support_GBK==1) && (_GBK_USE_MALLOC==1))  /* ֧��������ʾ */
	u16 gbkbufsize;
#endif
	
	u8 ascii_w;  /* Ӣ���ַ���� */
	u8 ascii_h;  /* Ӣ���ַ��߶� */
	
#if _LCD_Support_GBK==1
	u8 gbk_w;    /* �����ַ���� */
	u8 gbk_h;    /* �����ַ��߶� */
#endif
	
	u8 Scan_dir;  /* ��ʾɨ�跽�� */
}lcd_dev;

extern lcd_dev LcdDev;
/******************************************************************************
                               printf�����õ��ṹ
******************************************************************************/

typedef struct
{
	u16 cursorx;  //��굱ǰλ��
	u16 cursory;
	u16 cursorx0;  //������λ��
	u16 cursory0;
	u16 min_width;  //��ʾ��С��ȣ�С��ʱ����С��������
	u16 precision;  //��ʾ����
	u8  mode;  //ģʽ
	u8  charcnt;  //��ʾ�ַ�����
}_Typedef_lcdprintf;

extern _Typedef_lcdprintf lcd_printf;

#define ASCII1206	(u16)1206
#define ASCII1608	(u16)1608
#define ASCII2412	(u16)2412
#define GBK1212		(u16)1212
#define GBK1616		(u16)1616
#define GBK2424		(u16)2424

u32 LCD_Pow(u8 m,u8 n);
void LCD_Set_CharSize(u16 CharSize);
void LCD_Set_GBKSize(u16 GBKSize);
void Printf_Goto_xyPhys(u16 xPhys, u16 yPhys);
void LCD_printf_Init(void);
int LCD_printf(const u8 *format/* ��ʽ������ */, .../* ������ */);

#endif
