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
							��������ֿ�
*************************************************************************/
#if _LCD_Support_GBK==1
#include "MYTEXT.H"
#include "fontupd.h"
#endif

/**
  *****************************************************************************
  * @Name   : �����ַ���С
  *
  * @Brief  : none
  *
  * @Input  : CharSize: �ַ���С
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
  * @Name   : ���ú��ִ�С
  *
  * @Brief  : none
  *
  * @Input  : GBKSize: ���ִ�С
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
  * @Name   : ��ȡ�ַ�������
  *
  * @Brief  : none
  *
  * @Input  : *Str: Ҫ���㳤�ȵ��ַ���
  *
  * @Output : none
  *
  * @Return : �����ַ����ĳ��ȣ�������������
  *****************************************************************************
**/
u16 Get_StrLen(const u8 *Str)
{
	u16 i=0;
	
	if(*Str == 0)  return 0;
	
	while(1)
	{
		if(*Str == '\0')  return i+1;  //�����ַ����������ˣ����Ͻ���������������
		Str++;
		i++;
	}
}
/**
 *************************************************************************
 * @Name   : lcdָ��λ����ʾ������
 *
 * @Brief  : Ĭ�����λΪ0ʱ����ʾ��С����ǰһλ���������ǲ���0����ʾ
 *
 * @Input  : (x, y): �������
 *           Arr:    Ҫ��ʾ���֡���ʾ���ַ�Χ: 0 ~ 4294967295
 *           TLen:   ��ʾ�������ܳ���
 *           FLen:   ��ʾС��λ��
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
	
	//��ʾС������
	LCD_ShowxNum(x+((TLen-FLen)*LcdDev.ascii_w)+LcdDev.ascii_w, y, Arr, FLen, LcdDev.ascii_h, 0);
	
	//��ʾС����
	LCD_ShowChar(x+((TLen-FLen)*LcdDev.ascii_w), y, '.', LcdDev.ascii_h, 0);
	
	//��ʾ��������
	m = LCD_Pow(10, FLen);
	LCD_ShowxNum(x, y, Arr/m, TLen-FLen, LcdDev.ascii_h, 0);
}

/***********printf��ʽ�������Һ����غ���***********/
_Typedef_lcdprintf lcd_printf =
{
	0,  //��굱ǰλ��
	0,
	0,  //������ʾ��ʼλ��
	0,
	7,  //��С��ȣ�Ĭ����С7λ���
	3,  //���ȣ�Ĭ��3λС������
	0,  //��ʾģʽ����������ʾ
	0,  //����ַ�����
};
/**
  *****************************************************************************
  * @Name   : printf����x, y����
  *
  * @Brief  : none
  *
  * @Input  : xPhys: ����
  *           yPhys: ����
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
  * @Name   : printf��ȡx����
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : x��ǰ����ֵ
  *****************************************************************************
**/
u16 Printf_Get_xPhys(void)
{
	return lcd_printf.cursorx;
}

/**
  *****************************************************************************
  * @Name   : printf��ȡy����
  *
  * @Brief  : none
  *
  * @Input  : none
  *
  * @Output : none
  *
  * @Return : y��ǰ����ֵ
  *****************************************************************************
**/
u16 Printf_Get_yPhys(void)
{
	return lcd_printf.cursory;
}
/**
  *****************************************************************************
  * @Name   : printf��ʼ��
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
	LCD_Clear(BLACK);	//LCD����
	POINT_COLOR = RED;	//����������ɫ
	BACK_COLOR = BLACK;	//���ñ�����ɫ
	LCD_Set_CharSize(ASCII1608);	//�����ַ�����
#if _LCD_Support_GBK == 1
	LCD_Set_GBKSize(GBK1616);		//������������
#endif
	LcdDev._lcdobj.width = 240;		//����Һ�������
	LcdDev._lcdobj.height = 320;	//����Һ�����߶�
	Printf_Goto_xyPhys(0,0);		//������ʼ����ַ
}
/**
  *****************************************************************************
  * @Name   : ��ȡ��ֵ����
  *
  * @Brief  : ����0���ص�λ��Ϊ0�����������ֵΪ0����λΪ0�Ĳ��������ڣ�Ĭ�ϴӸ�λ��ʼ����
  *
  * @Input  : Num: Ҫ������ֵ����ֵ��Χ: 0 ~ 4294967295
  *
  * @Output : none
  *
  * @Return : ��ֵ���ȣ�Ĭ����󷵻�10λ
  *****************************************************************************
**/
#define MAX_NUM_LEN          10    //��󷵻�λ��

u8 Get_NumLen(u32 Num)
{
	u8 i=0;
	u8 tmp=0;

	i = MAX_NUM_LEN - 1;    //�õ�����λ��
	while(1)
	{
		tmp = (Num/LCD_Pow(10, i)) % 10;    //�õ�λ��
		if(tmp || i==0)    break;    //���
		i--;
	}
	     if(i)    return i+1;    //������ֵλ��
	else if(tmp)  return 1;    //�������ֵֻ��һλ
	else          return 0;    //�������0
}
/**
  *****************************************************************************
  * @Name   : ������ת�����ַ���
  *
  * @Brief  : none
  *
  * @Input  : arr:    Ҫת��������
  *           strout: �ַ����������
  *
  * @Output : none
  *
  * @Return : �ַ������ָ��
  *****************************************************************************
**/
u8 *Array_To_String(u32 arr, u8* strout)
{
	u8 tmp=0,i=0;
	u8 rec=0;

	for(i = 0;i < 10;i++)
	{
		tmp = (arr / (LCD_Pow(10, 10-i-1))) % 10;    //ȡ�����ֵ����λ
		if(rec==0 && i<9)  //���һλ�����ǲ���0��ת��
		{
			if(tmp == 0)	continue;  //��λΪ0��ת��
			else  rec = 1;  //����Ҫת����
		}
		*strout = tmp + 0x30;
		strout++;
	}
	*strout = '\0';	//���������

	return strout;
}
/**
  *****************************************************************************
  * @Name   : printf����ַ�
  *
  * @Brief  : none
  *
  * @Input  : lcd_ch: printf�ṹ��ָ��
  *           ch:     Ҫ������ַ�
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
		lcd_ch->cursorx += (LcdDev.ascii_w * 3);  //Ĭ��4���ַ����
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
  * @Name   : printf����ַ���
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf�ṹ��ָ��
  *           *str:   Ҫ������ַ���
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
#if _LCD_Support_GBK==1  //֧������
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
  * @Name   : printf�������
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf�ṹ��ָ��
  *           dec:    Ҫ���������
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
	else if(tmp < 0)  //����
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
  * @Name   : printf���������
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf�ṹ��ָ��
  *           flt:    Ҫ����ĸ�����
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
	
	i = pre = (int)LCD_Pow(10, lcd_dec->precision);  //�õ�С������
	i *= 10;
	flt_flo = (int)(tmp * i);  //ת������������
	i = flt_flo % 10;
	if((i <= 9) && (i >= 5))
	{
		flt_flo += 10;  //��������
		flt_flo /= 10;
	}
	else  flt_flo /= 10;
	
	flt_int = flt_flo / pre;  //�õ�����λ
	flt_flo = flt_flo % pre;  //�õ�С��λ
	
	_printfint(lcd_dec, flt_int%pre);  //��ʾ����λ
	_printfch(lcd_dec, '.');
	lcd_dec->cursorx += LcdDev.ascii_w;
	for(i = 0;i < lcd_dec->precision;i++)  //��ʾС��������0
	{
		flt_int = (flt_flo / (LCD_Pow(10, (lcd_dec->precision-i-1)))) % 10;  //ȡ�����ֵ����λ
		if(flt_int == 0)
		{
			_printfch(lcd_dec, 0x30);  //С��������0����ʾ
			lcd_dec->cursorx += LcdDev.ascii_w;
		}
		else  break;
	}
	_printfint(lcd_dec, flt_flo);  //��ʾС��λ
}
/**
  *****************************************************************************
  * @Name   : printf���������
  *
  * @Brief  : ֱ��������ת���С����ʾ
  *
  * @Input  : lcd_dec: printf�ṹ��ָ��
  *           itf:     Ҫ���������������
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
	itf_int = temp / itf_flo;  //�õ�����
	itf_flo = temp % itf_flo;  //�õ�С��
	
	_printfint(lcd_dec, itf_int);
	_printfch(lcd_dec, '.');
	lcd_dec->cursorx += LcdDev.ascii_w;
	for(i = 0;i < lcd_dec->precision;i++)  //��ʾС��������0
	{
		itf_int = (itf_flo / (LCD_Pow(10, (lcd_dec->precision-i-1)))) % 10;  //ȡ�����ֵ����λ
		if(itf_int == 0)
		{
			_printfch(lcd_dec, 0x30);  //С��������0����ʾ
			lcd_dec->cursorx += LcdDev.ascii_w;
		}
		else  break;
	}
	_printfint(lcd_dec, itf_flo);  //��ʾС��λ
}
/**
  *****************************************************************************
  * @Name   : printf���ʮ��������
  *
  * @Brief  : none
  *
  * @Input  : lcd_st: printf�ṹ��ָ��
  *           Hex:    Ҫ�����ʮ��������
  *           index:  Ҫת���Ľ�������16: ʮ�����ƣ�8: �˽���
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
	
	//�ַ�������
	for(j = 0;j <=(i-1)/2;j++)  //= --> bug
	{
		tmp = str[j];
		str[j] = str[i-1-j];
		str[i-1-j] = tmp;
	}
	
	if(index == 16)  //ʮ������
	{
		_printfch(lcd_hex, '0');lcd_hex->cursorx += LcdDev.ascii_w;
		_printfch(lcd_hex, 'x');lcd_hex->cursorx += LcdDev.ascii_w;
	}
	else if(index == 8)  //�˽���
	{
		_printfch(lcd_hex, '0');lcd_hex->cursorx += LcdDev.ascii_w;
	}
	_printfstr(lcd_hex, (const char *)str);
}
/**
  *****************************************************************************
  * @Name   : printf��ʽ�������Һ��
  *
  * @Brief  : none
  *
  * @Input  : lcd_ch: printf�ṹ��ָ��
  *           format: ��ʽ���ַ���
  *           ...:    ������
  *
  * @Output : none
  *
  * @Return : none
  *****************************************************************************
**/
int LCD_printf(const u8 *format/* ��ʽ������ */, .../* ������ */)
{
	const char *str;
	char c=0;
	float valflt=0;
	int dec=0;
	u8 index=0;
	_Typedef_lcdprintf* lcd_pr = &lcd_printf;		//�ṹ����
	va_list ap;
	va_start(ap, format);
	
	lcd_pr->min_width = 7;
	lcd_pr->precision = 3;
	lcd_pr->charcnt   = 0;
	
	while(*format != '\0')  //ֱ������������
	{
		if(*format == 0x5c)  //����'\'
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
		else if(*format == '%')  //����ʽ���
		{
			format++;
			index = 0;
			lcd_pr->min_width = 0;
			while((*format <= '9') && (*format >= '0'))  //���
			{
				lcd_pr->min_width = (lcd_pr->min_width * index * 10) + (*format - '0');
				index++;
				format++;
			}
			if(*format == '.')  //��⵽��
			{
				format++;
				index = 0;
				lcd_pr->precision = 0;
				while((*format <= '9') && (*format >= '0'))  //����
				{
					lcd_pr->precision = (lcd_pr->precision * index * 10) + (*format - '0');
					index++;
					format++;
				}
			}
			switch(*format)  //����ʽ��
			{
				case '%':  //���%����
						_printfch(lcd_pr, '%');
						lcd_pr->cursorx += LcdDev.ascii_w;
						format++;
						break;
				
				case 'c':  //����ַ�
						c = va_arg(ap, int);
						_printfch(lcd_pr, c);
						format++;
						break;
				
				case 'S':
				case 's':  //����ַ���
						str = va_arg(ap, const char *);
						_printfstr(lcd_pr, (char*)str);
						format++;
						break;
						
				case 'd':  //���ʮ������
						dec = va_arg(ap, int);
						_printfint(lcd_pr, dec);
						format++;
						break;
				
				case 'f':  //���������
						valflt = va_arg(ap, double);
						_printfflt(lcd_pr, valflt);
						format++;
						break;
				
				case 'X':
				case 'x':  //���ʮ��������
						dec = va_arg(ap, int);
						_printfhex(lcd_pr, dec, 16);
						format++;
						break;
				
				case 'o':  //����˽�����
						dec = va_arg(ap, int);
						_printfhex(lcd_pr, dec, 8);
						format++;
						break;
				
				case 'm':
						dec = va_arg(ap, int);
						if(dec > 21)  dec = 21;  //�������������21������С����
						lcd_pr->min_width = (u16)dec;
						format++;
						if(*format == '.')
						{
							format++;
							if(*format == 'n')
							{
								dec = va_arg(ap, int);
								if(dec > 8)  dec = 8;  //������С��С�������ǣ�0.00000001
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
		else if(*format < 0x80)  //�ַ����
		{
			_printfch(lcd_pr, *format);
			lcd_pr->cursorx += LcdDev.ascii_w;  //��һ���ַ�λ��
			format++;
		}
#if _LCD_Support_GBK==1  //֧������
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

