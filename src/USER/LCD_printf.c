/**
 ************************************************************************
 * @file	: LCD_printf.c
 * @by		:
 * @version	: V1.0
 * @Date    : 2014 / 10 / 5
 *************************************************************************
**/
#include "LCD_printf.h"
#include "stdarg.h"

lcd_dev LcdDev;
char HexIndex[16] = "0123456789ABCDEF";
/*************************************************************************
							添加中文字库
*************************************************************************/
#if _LCD_Support_GBK==1
#include "MYTEXT.H"
#include "fontupd.h"
#endif

/**
  *****************************************************************************
  * @Name   : 设置字符大小
  *
  * @Brief  : none
  *
  * @Input  : CharSize: 字符大小
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void LCD_Set_CharSize(u16 CharSize)
{
	LcdDev.ascii_h = CharSize / 100;
	LcdDev.ascii_w = CharSize % 100;
}
#if _LCD_Support_GBK==1
/**
  *****************************************************************************
  * @Name   : 设置汉字大小
  *
  * @Brief  : none
  *
  * @Input  : GBKSize: 汉字大小
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void LCD_Set_GBKSize(u16 GBKSize)
{
	LcdDev.gbk_h = GBKSize / 100;
	LcdDev.gbk_w = GBKSize % 100;
}

#endif
/**
  *****************************************************************************
  * @Name   : 获取字符串长度
  *
  * @Brief  : none
  *
  * @Input  : *Str: 要计算长度的字符串
  *
  * @Output : none
  *
  * @Return : 返回字符串的长度，包含结束符号
  *****************************************************************************
**/
u16 Get_StrLen(const u8 *Str)
{
	u16 i=0;
	
	if(*Str == 0)  return 0;
	
	while(1)
	{
		if(*Str == '\0')  return i+1;  //遇到字符串结束符了，加上结束符数量并返回
		Str++;
		i++;
	}
}
/**
 *************************************************************************
 * @Name   : lcd指定位置显示浮点数
 *
 * @Brief  : 默认最高位为0时不显示，小数点前一位整数不管是不是0都显示
 *
 * @Input  : (x, y): 起点坐标
 *           Arr:    要显示数字。显示数字范围: 0 ~ 4294967295
 *           TLen:   显示浮点数总长度
 *           FLen:   显示小数位数
 *
 * @Output : none
 *
 * @Return : none
 *************************************************************************
**/
void LCD_Show_Float(u16 x, u16 y, u32 Arr, u8 TLen, u8 FLen)
{
	u32 m=0;
	
	if((x > LcdDev._lcdobj.width) || (y > LcdDev._lcdobj.height))  return;
	
	//显示小树部分
	LCD_ShowxNum(x+((TLen-FLen)*LcdDev.ascii_w)+LcdDev.ascii_w, y, Arr, FLen, LcdDev.ascii_h, 0);
	
	//显示小数点
	LCD_ShowChar(x+((TLen-FLen)*LcdDev.ascii_w), y, '.', LcdDev.ascii_h, 0);
	
	//显示整数部分
	m = LCD_Pow(10, FLen);
	LCD_ShowxNum(x, y, Arr/m, TLen-FLen, LcdDev.ascii_h, 0);
}

/***********printf格式化输出到液晶相关函数***********/
_Typedef_lcdprintf lcd_printf =
{
	0,  //光标当前位置
	0,
	0,  //设置显示起始位置
	0,
	7,  //最小宽度，默认最小7位宽度
	3,  //精度，默认3位小数精度
	0,  //显示模式，不叠加显示
	0,  //输出字符清零
};
/**
  *****************************************************************************
  * @Name   : printf设置x, y坐标
  *
  * @Brief  : none
  *
  * @Input  : xPhys: 坐标
  *           yPhys: 坐标
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void Printf_Goto_xyPhys(u16 xPhys, u16 yPhys)
{
	lcd_printf.cursorx  = xPhys;
	lcd_printf.cursory  = yPhys;
	lcd_printf.cursorx0 = lcd_printf.cursorx;
	lcd_printf.cursory0 = lcd_printf.cursory;
}

/**
  *****************************************************************************
  * @Name   : printf获取x坐标
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : x当前坐标值
  *****************************************************************************
**/
u16 Printf_Get_xPhys(void)
{
	return lcd_printf.cursorx;
}

/**
  *****************************************************************************
  * @Name   : printf获取y坐标
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : y当前坐标值
  *****************************************************************************
**/
u16 Printf_Get_yPhys(void)
{
	return lcd_printf.cursory;
}
/**
  *****************************************************************************
  * @Name   : printf初始化
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void LCD_printf_Init(void)
{
	LCD_Clear(BLACK);	//LCD清屏
	POINT_COLOR = RED;	//配置字体颜色
	BACK_COLOR = BLACK;	//配置背景颜色
	LCD_Set_CharSize(ASCII1608);	//设置字符字体
#if _LCD_Support_GBK == 1
	LCD_Set_GBKSize(GBK1616);		//设置中文字体
#endif
	LcdDev._lcdobj.width = 240;		//设置液晶屏宽度
	LcdDev._lcdobj.height = 320;	//设置液晶屏高度
	Printf_Goto_xyPhys(0,0);		//设置起始光标地址
}
/**
  *****************************************************************************
  * @Name   : 获取数值长度
  *
  * @Brief  : 数字0返回的位数为0，即输入的数值为0。高位为0的不计算在内，默认从高位开始计算
  *
  * @Input  : Num: 要检测的数值。数值范围: 0 ~ 4294967295
  *
  * @Output : none
  *
  * @Return : 数值长度，默认最大返回10位
  *****************************************************************************
**/
#define MAX_NUM_LEN          10    //最大返回位数

u8 Get_NumLen(u32 Num)
{
	u8 i=0;
	u8 tmp=0;

	i = MAX_NUM_LEN - 1;    //得到返回位数
	while(1)
	{
		tmp = (Num/LCD_Pow(10, i)) % 10;    //得到位数
		if(tmp || i==0)    break;    //完成
		i--;
	}
	     if(i)    return i+1;    //返回数值位数
	else if(tmp)  return 1;    //输入的数值只有一位
	else          return 0;    //输入的是0
}
/**
  *****************************************************************************
  * @Name   : 将数字转换成字符串
  *
  * @Brief  : none
  *
  * @Input  : arr:    要转换的数字
  *           strout: 字符串输出缓存
  *
  * @Output : none
  *
  * @Return : 字符串输出指针
  *****************************************************************************
**/
u8 *Array_To_String(u32 arr, u8* strout)
{
	u8 tmp=0,i=0;
	u8 rec=0;

	for(i = 0;i < 10;i++)
	{
		tmp = (arr / (LCD_Pow(10, 10-i-1))) % 10;    //取得数字的最高位
		if(rec==0 && i<9)  //最后一位不管是不是0都转换
		{
			if(tmp == 0)	continue;  //高位为0不转换
			else  rec = 1;  //不需要转换了
		}
		*strout = tmp + 0x30;
		strout++;
	}
	*strout = '\0';	//加入结束符

	return strout;
}
/**
  *****************************************************************************
  * @Name   : printf输出字符
  *
  * @Brief  : none
  *
  * @Input  : lcd_ch: printf结构体指针
  *           ch:     要输出的字符
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void _printfch(_Typedef_lcdprintf* lcd_ch, const char ch)
{	
	if(ch == '\r')  //LF
	{
		lcd_ch->cursorx  = 0;
		lcd_ch->cursory += LcdDev.ascii_h;
		if(lcd_ch->cursory >= LcdDev._lcdobj.height)  lcd_ch->cursory = 0;
	}
	else if(ch == '\n')  //CR
	{
		lcd_ch->cursorx  = lcd_ch->cursorx0 - LcdDev.ascii_w;
		lcd_ch->cursory += LcdDev.ascii_h;
		if(lcd_ch->cursory >= LcdDev._lcdobj.height)  lcd_ch->cursory = 0;
	}
	else if(ch == '\t')  //Tab
	{
		lcd_ch->cursorx += (LcdDev.ascii_w * 3);  //默认4个字符宽度
		if(lcd_ch->cursorx >= LcdDev._lcdobj.width)  lcd_ch->cursorx = 0;
	}
	else  //en
	{
		LCD_ShowChar(lcd_ch->cursorx, lcd_ch->cursory, ch, LcdDev.ascii_h, (u8)(lcd_ch->mode&0x1));
		lcd_ch->charcnt++;
	}
}
/**
  *****************************************************************************
  * @Name   : printf输出字符串
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf结构体指针
  *           *str:   要输出的字符串
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void _printfstr(_Typedef_lcdprintf* lcd_st, const char *str)
{
	for(;*str != '\0';str++)
	{
		if(*str < (char)0x80)  //en
		{
			_printfch(lcd_st, *str);
			lcd_st->cursorx += LcdDev.ascii_w;
			if(lcd_st->cursorx >= LcdDev._lcdobj.width)  lcd_st->cursory += LcdDev.ascii_h;
			if(lcd_st->cursory >= LcdDev._lcdobj.height)  {lcd_st->cursory = 0;lcd_st->cursorx = 0;}
		}
#if _LCD_Support_GBK==1  //支持中文
		else
		{
			Show_Font(lcd_st->cursorx, lcd_st->cursory, (u8*)str, LcdDev.gbk_h, (u8)(lcd_st->mode&1));
			lcd_st->cursorx += LcdDev.gbk_w;
			str++;
			lcd_st->charcnt++;
		}
#endif
	}
}
/**
  *****************************************************************************
  * @Name   : printf输出数字
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf结构体指针
  *           dec:    要输出的数字
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void _printfint(_Typedef_lcdprintf* lcd_dec, const int dec)
{
	char st[10];
	int tmp = dec;
	
	if(tmp == 0)  return;
	else if(tmp < 0)  //负数
	{
		tmp = -tmp;
		_printfch(lcd_dec, '-');
		lcd_dec->cursorx += LcdDev.ascii_w;
	}
	
	Array_To_String    ((u32)tmp, (u8*)st);
	_printfstr(lcd_dec, (const char *)st);
}

/**
  *****************************************************************************
  * @Name   : printf输出浮点数
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf结构体指针
  *           flt:    要输出的浮点数
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void _printfflt(_Typedef_lcdprintf* lcd_dec, const float flt)
{
	int pre=0,i=0;
	int flt_int=0;
	int flt_flo=0;
	float tmp=flt;
	
	if(tmp < 0)
	{
		tmp = -tmp;
		_printfch(lcd_dec, '-');
		lcd_dec->cursorx += LcdDev.ascii_w;
	}
	
	i = pre = (int)LCD_Pow(10, lcd_dec->precision);  //得到小数精度
	i *= 10;
	flt_flo = (int)(tmp * i);  //转换成整数处理
	i = flt_flo % 10;
	if((i <= 9) && (i >= 5))
	{
		flt_flo += 10;  //四舍五入
		flt_flo /= 10;
	}
	else  flt_flo /= 10;
	
	flt_int = flt_flo / pre;  //得到整数位
	flt_flo = flt_flo % pre;  //得到小数位
	
	_printfint(lcd_dec, flt_int%pre);  //显示整数位
	_printfch(lcd_dec, '.');
	lcd_dec->cursorx += LcdDev.ascii_w;
	for(i = 0;i < lcd_dec->precision;i++)  //显示小数点后面的0
	{
		flt_int = (flt_flo / (LCD_Pow(10, (lcd_dec->precision-i-1)))) % 10;  //取得数字的最高位
		if(flt_int == 0)
		{
			_printfch(lcd_dec, 0x30);  //小数点后面的0都显示
			lcd_dec->cursorx += LcdDev.ascii_w;
		}
		else  break;
	}
	_printfint(lcd_dec, flt_flo);  //显示小数位
}
/**
  *****************************************************************************
  * @Name   : printf输出浮点数
  *
  * @Brief  : 直接由整数转变成小数显示
  *
  * @Input  : lcd_dec: printf结构体指针
  *           itf:     要输出浮点数的整数
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void _printfitf(_Typedef_lcdprintf* lcd_dec, const int itf)
{
	int itf_int=0,itf_flo=0,i=0;
	int temp=itf;
	
	if(temp < 0)
	{
		temp = -temp;
		_printfch(lcd_dec, '-');
		lcd_dec->cursorx += LcdDev.ascii_w;
	}
	
	itf_flo = (int)LCD_Pow(10, lcd_dec->precision);
	itf_int = temp / itf_flo;  //得到整数
	itf_flo = temp % itf_flo;  //得到小数
	
	_printfint(lcd_dec, itf_int);
	_printfch(lcd_dec, '.');
	lcd_dec->cursorx += LcdDev.ascii_w;
	for(i = 0;i < lcd_dec->precision;i++)  //显示小数点后面的0
	{
		itf_int = (itf_flo / (LCD_Pow(10, (lcd_dec->precision-i-1)))) % 10;  //取得数字的最高位
		if(itf_int == 0)
		{
			_printfch(lcd_dec, 0x30);  //小数点后面的0都显示
			lcd_dec->cursorx += LcdDev.ascii_w;
		}
		else  break;
	}
	_printfint(lcd_dec, itf_flo);  //显示小数位
}
/**
  *****************************************************************************
  * @Name   : printf输出十六进制数
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf结构体指针
  *           Hex:    要输出的十六进制数
  *           index:  要转换的进制数。16: 十六进制；8: 八进制
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
void _printfhex(_Typedef_lcdprintf* lcd_hex, const int Hex, int index)
{
	char str[16];
	char tmp=0;
	int i = 0,j = 0;
	int temp=Hex;
	
	do
	{
		str[i++] = HexIndex[temp%index];
		temp /= index;
	}while(temp);
	str[i] = '\0';
	
	//字符串倒序
	for(j = 0;j <=(i-1)/2;j++)  //= --> bug
	{
		tmp = str[j];
		str[j] = str[i-1-j];
		str[i-1-j] = tmp;
	}
	
	if(index == 16)  //十六进制
	{
		_printfch(lcd_hex, '0');lcd_hex->cursorx += LcdDev.ascii_w;
		_printfch(lcd_hex, 'x');lcd_hex->cursorx += LcdDev.ascii_w;
	}
	else if(index == 8)  //八进制
	{
		_printfch(lcd_hex, '0');lcd_hex->cursorx += LcdDev.ascii_w;
	}
	_printfstr(lcd_hex, (const char *)str);
}
/**
  *****************************************************************************
  * @Name   : printf格式化输出到液晶
  *
  * @Brief  : none
  *
  * @Input  : lcd_ch: printf结构体指针
  *           format: 格式化字符串
  *           ...:    长参数
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
int LCD_printf(const u8 *format/* 格式化参数 */, .../* 长参数 */)
{
	const char *str;
	char c=0;
	float valflt=0;
	int dec=0;
	u8 index=0;
	_Typedef_lcdprintf* lcd_pr = &lcd_printf;		//结构参数
	va_list ap;
	va_start(ap, format);
	
	lcd_pr->min_width = 7;
	lcd_pr->precision = 3;
	lcd_pr->charcnt   = 0;
	
	while(*format != '\0')  //直到遇到结束符
	{
		if(*format == 0x5c)  //符号'\'
		{
			format++;
			switch(*format)
			{
				case 'r':  //LF
						_printfch(lcd_pr, '\r');
						format++;
						break;
				
				case 'n':  //CR
						_printfch(lcd_pr, '\n');
						format++;
						break;
				
				case 't':  //Tab
						_printfch(lcd_pr, '\t');
						format++;
						break;
				
				default:
						format++;
						break;
			}
		}
		else if(*format == '%')  //带格式输出
		{
			format++;
			index = 0;
			lcd_pr->min_width = 0;
			while((*format <= '9') && (*format >= '0'))  //域宽
			{
				lcd_pr->min_width = (lcd_pr->min_width * index * 10) + (*format - '0');
				index++;
				format++;
			}
			if(*format == '.')  //检测到点
			{
				format++;
				index = 0;
				lcd_pr->precision = 0;
				while((*format <= '9') && (*format >= '0'))  //精度
				{
					lcd_pr->precision = (lcd_pr->precision * index * 10) + (*format - '0');
					index++;
					format++;
				}
			}
			switch(*format)  //检测格式码
			{
				case '%':  //输出%符号
						_printfch(lcd_pr, '%');
						lcd_pr->cursorx += LcdDev.ascii_w;
						format++;
						break;
				
				case 'c':  //输出字符
						c = va_arg(ap, int);
						_printfch(lcd_pr, c);
						format++;
						break;
				
				case 'S':
				case 's':  //输出字符串
						str = va_arg(ap, const char *);
						_printfstr(lcd_pr, (char*)str);
						format++;
						break;
						
				case 'd':  //输出十进制数
						dec = va_arg(ap, int);
						_printfint(lcd_pr, dec);
						format++;
						break;
				
				case 'f':  //输出浮点数
						valflt = va_arg(ap, double);
						_printfflt(lcd_pr, valflt);
						format++;
						break;
				
				case 'X':
				case 'x':  //输出十六进制数
						dec = va_arg(ap, int);
						_printfhex(lcd_pr, dec, 16);
						format++;
						break;
				
				case 'o':  //输出八进制数
						dec = va_arg(ap, int);
						_printfhex(lcd_pr, dec, 8);
						format++;
						break;
				
				case 'm':
						dec = va_arg(ap, int);
						if(dec > 21)  dec = 21;  //限制最大的域宽是21，包括小数点
						lcd_pr->min_width = (u16)dec;
						format++;
						if(*format == '.')
						{
							format++;
							if(*format == 'n')
							{
								dec = va_arg(ap, int);
								if(dec > 8)  dec = 8;  //限制最小的小数精度是，0.00000001
								lcd_pr->precision = (u16)dec;
								format++;
								if(*format == 'f')
								{
									valflt = va_arg(ap, double);
									_printfflt(lcd_pr, valflt);
									format++;
								}
							}
						}
						break;
				
				default:
						_printfch(lcd_pr, *format);
						format++;
						break;
			}  //end check format
		}
		else if(*format < 0x80)  //字符输出
		{
			_printfch(lcd_pr, *format);
			lcd_pr->cursorx += LcdDev.ascii_w;  //下一个字符位置
			format++;
		}
#if _LCD_Support_GBK==1  //支持中文
		else
		{
			Show_Font(lcd_pr->cursorx, lcd_pr->cursory, (u8*)format++, LcdDev.gbk_h, (u8)lcd_pr->mode);
			lcd_pr->cursorx += LcdDev.gbk_w;
			format++;
			lcd_pr->charcnt++;
		}
#endif
	}  //end while
	va_end(ap);
	
	return (int)lcd_pr->charcnt;
}

