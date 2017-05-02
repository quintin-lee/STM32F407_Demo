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

//定义_LCD_Support_GBK 如果为1,则支持中文,否则不支持
#define  _LCD_Support_GBK	1

typedef struct
{
	_lcd_dev _lcdobj;
	
#if ((_LCD_Support_GBK==1) && (_GBK_USE_MALLOC==1))  /* 支持中文显示 */
	u16 gbkbufsize;
#endif
	
	u8 ascii_w;  /* 英文字符宽度 */
	u8 ascii_h;  /* 英文字符高度 */
	
#if _LCD_Support_GBK==1
	u8 gbk_w;    /* 中文字符宽度 */
	u8 gbk_h;    /* 中文字符高度 */
#endif
	
	u8 Scan_dir;  /* 显示扫描方向 */
}lcd_dev;

extern lcd_dev LcdDev;
/******************************************************************************
                               printf函数用到结构
******************************************************************************/

typedef struct
{
	u16 cursorx;  //光标当前位置
	u16 cursory;
	u16 cursorx0;  //保存光标位置
	u16 cursory0;
	u16 min_width;  //显示最小宽度，小数时包括小数点在内
	u16 precision;  //显示精度
	u8  mode;  //模式
	u8  charcnt;  //显示字符计数
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
int LCD_printf(const u8 *format/* 格式化参数 */, .../* 长参数 */);

#endif
