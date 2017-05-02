#include "sys.h"
#include "bsp_led.h"
#include "bsp_lcd.h"
#include "delay.h"
#include "usart.h"
#include "bsp_myiic.h"
#include "bsp_24cxx.h"
#include "bsp_touch.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_w25qxx.h"
#include "exfuns.h"
#include "fontupd.h"
#include "mytext.h"
#include "bsp_hc05.h"
#include "bsp_rtc.h"
#include "bsp_tpad.h"
#include "piclib.h"
#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h" 

/**
 * STM32F407Сϵͳ���Գ���	V2.1
 */
 
USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB״̬
extern vu8 bDeviceState;		//USB���� ���
 
//������
int main(void)
{
	u8 key;
	RTC_TimeTypeDef RTC_TimeStruct;
	RTC_DateTypeDef RTC_DateStruct;
	u8 tbuf[40];
	u8 t=0;
	u32 led0sta=0;
	DIR picdir;
	u8 res;
	u32 dtsize,dfsize;
	u8 temp;
	u8 USB_STA;
	u8 Divece_STA;
	u8 offline_cnt=0;
	u8 tct=0;
	
	/* �ж����ȼ����� */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* BSP��ʼ�� */
	delay_init(168);		//ϵͳʱ�ӳ�ʼ��
	uart_init(115200);		//����1��ʼ��
	LED_Initialize();		//LED��ʼ��
	TPAD_Init(8);			//����������ʼ��
	TFTLCD_Init(BLACK);		//TFTLCD��ʼ��
	TP_Init();				//��������ʼ��
	SD_Init();				//SD��ʼ��
	W25QXX_Init();			//W25Qxx��ʼ��
	My_RTC_Init();			//RTC��ʼ��
	piclib_init();			//��ʼ����ͼ
	
	/* FATFS�����ڴ� */
	exfuns_init();
	
	/* ����FLASH SD */
	f_mount(fs[0], "0:", 1);
	f_mount(fs[1], "1:", 1);
		
	/* ��ȡFLASH������������ */
	do
	{
		temp++;
 		res=exf_getfree((u8*)"1:",&dtsize,&dfsize);
		delay_ms(200);		   
	}while(res&&temp<20);

	/* �ļ�ϵͳ������,��ʽ��FLASH */
	if(res==0X0D)
	{
		LCD_ShowString(5,30,240,16,16, (u8*)"Flash Disk Formatting...");	//��ʽ��FLASH
		res=f_mkfs("1:",1,4096);//��ʽ��FLASH,1,�̷�;1,����Ҫ������,8������Ϊ1����
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:DZXZ");		//����Flash���̵�����Ϊ��DZXZ
			LCD_ShowString(5,30+16,240,16,16, (u8*)"OK");//��־��ʽ���ɹ�
 			res=exf_getfree((u8*)"1:",&dtsize,&dfsize);//���»�ȡ����
		}
	}
	
	/* ����ֿ� */
	while(font_init())
	{
		LCD_Clear(WHITE);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		LCD_ShowString(30,50,200,16,16,(u8*)"STM32F407ZET6");
		while(SD_Init())			//���SD��
		{
			LCD_ShowString(30,70,200,16,16,(u8*)"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			delay_ms(200);		    
		}
		LCD_ShowString(30,70,200,16,16,(u8*)"SD Card OK");
		LCD_ShowString(30,90,200,16,16,(u8*)"Font Updating...");
		key=update_font(20,110,16,(u8*)"0:");//�����ֿ�
		while(key)//����ʧ��		
		{			 		  
			LCD_ShowString(30,110,200,16,16,(u8*)"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,(u8*)"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(BLACK);//����
	}
	
	/* ��ͼƬ�ļ��� */
	res=f_opendir(&picdir,(const TCHAR*)"1:/PICTURE"); 	//��Ŀ¼
	if (res==FR_OK)//�򿪳ɹ�
	{
		char i;
		LCD_Clear(BLACK);
		for (i = 0; i < 3; i++)
		{
			ai_load_picfile((const u8*)"1:/PICTURE/Start.gif",0,0,lcddev.width,lcddev.height,1);
		}
		delay_ms(2000);
		LCD_Clear(BLACK);
		ai_load_picfile((const u8*)"1:/PICTURE/LOGO.jpg",0,0,lcddev.width,lcddev.height,1);//��ʾͼƬ
		delay_ms(2000);
		delay_ms(2000);
	}
	
	//���SD���Ƿ����
	while (SD_Init())
	{
		LCD_ShowString(30,70,200,16,16,(u8*)"SD Card Failed!");
		LCD_ShowString(30,90,200,16,16,(u8*)"Please Insert SD Card!");
		delay_ms(200);
		LCD_Fill(30,70,200+30,70+16,BLACK);
		LCD_Fill(30,90,200+30,70+16,BLACK);
		delay_ms(200);
	}
START:	
	LCD_Clear(BLACK);
	BACK_COLOR = BLACK;
	POINT_COLOR = RED;
	Show_Str(20, 5, 240, 24, (u8*)"STM32F407���Գ���", 24, 0);
	POINT_COLOR=BLUE;
	LCD_printf(20, 35, "FLASH����������:%dKB", dtsize);
	LCD_printf(20, 55, "FLASH����ʣ����:%dKB", dfsize);
	
	/* ����ģ���� */
	if (HC05_Init())
	{
//		LCD_Clear(BACK_COLOR);		   	//����
 		POINT_COLOR=RED;			//��������Ϊ��ɫ	   	   	  
		Show_Str(20, 75, 200, 16, (u8*)"���������ڻ�����!", 16, 0);
	}
	else
	{
		Show_Str(20, 75, 200, 16, (u8*)"����ģ������        ", 16, 0);
	}

	POINT_COLOR = RED;
	Show_Str(20, 100, 200, 32, (u8*)"ʱ�Ӳ���", 32, 0);
	Show_Str(20, 220, 200, 40, (u8*)"����С��", 40, 0);
	POINT_COLOR=BLUE;
	while (1U)
	{
		/* �жϴ��������Ƿ��� */
		/* �ɹ�������һ��������(�˺���ִ��ʱ������15ms) */
		if (TPAD_Scan(0))
		{
			LCD_Clear(BACK_COLOR);
			LCD_printf(30, 170, "USB����...  ");
			USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
			delay_ms(1800);
			while (1)
			{
				delay_ms(1);
				if (USB_STA != USB_STATUS_REG)
				{
					LCD_Fill(30,190,240,190+16,BACK_COLOR);//�����ʾ			  	   
					if(USB_STATUS_REG&0x01)//����д		  
					{
						LED_On(1);
						LCD_printf(30,190,"USBд��...  ");//��ʾUSB����д������	 
					}
					if(USB_STATUS_REG&0x02)//���ڶ�
					{
						LED_On(1);
						LCD_printf(30,190,"USB��ȡ...  ");//��ʾUSB���ڶ�������  		 
					}	 										  
					if(USB_STATUS_REG&0x04)LCD_printf(30,210,"USBд����� ");//��ʾд�����
					else LCD_Fill(30,210,240,210+16,BACK_COLOR);//�����ʾ	  
					if(USB_STATUS_REG&0x08)LCD_printf(30,230,"USB��ȡ���� ");//��ʾ��������
					else LCD_Fill(30,230,240,230+16,BACK_COLOR);//�����ʾ    
					USB_STA=USB_STATUS_REG;//��¼����״̬
				}
				if(Divece_STA!=bDeviceState) 
				{
					if(bDeviceState==1)LCD_printf(30,170,"USB����      ");//��ʾUSB�����Ѿ�����
					else 
					{
						LCD_printf(30,170,"USB�Ͽ����� ");//��ʾUSB���γ���
					}
					Divece_STA=bDeviceState;
				}
				tct++;
				if(tct==200)
				{
					tct=0;
					LED_Off(1);
					if(USB_STATUS_REG&0x10)
					{
						offline_cnt=0;//USB������,�����offline������
						bDeviceState=1;
					}else//û�еõ���ѯ 
					{
						offline_cnt++;  
						if(offline_cnt>10)bDeviceState=0;//2s��û�յ����߱��,����USB���γ���
					}
					USB_STATUS_REG=0;
				}
				if (TPAD_Scan(0))
				{
					goto START;
				}
			}
		}	
		
		t++;
		if((t%10)==0)	//ÿ100ms����һ����ʾ����
		{
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
			
			sprintf((char*)tbuf,"ʱ��:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
			Show_Str(20,140,210,16,tbuf,16,0);	
			
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
			
			sprintf((char*)tbuf,"����:20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date); 
			Show_Str(20,160,210,16,tbuf,16,0);	
			sprintf((char*)tbuf,"����:%d",RTC_DateStruct.RTC_WeekDay); 
			Show_Str(20,180,210,16,tbuf,16,0);
		}
		
		if ((t%20) == 0)
		{
			led0sta = ~led0sta;
			LED_Out(led0sta&0x00000003);
		}
		delay_ms(10);
	}
}
