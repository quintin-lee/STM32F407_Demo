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
 * STM32F407小系统测试程序	V2.1
 */
 
USB_OTG_CORE_HANDLE USB_OTG_dev;
extern vu8 USB_STATUS_REG;		//USB状态
extern vu8 bDeviceState;		//USB连接 情况
 
//主函数
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
	
	/* 中断优先级分组 */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	/* BSP初始化 */
	delay_init(168);		//系统时钟初始化
	uart_init(115200);		//串口1初始化
	LED_Initialize();		//LED初始化
	TPAD_Init(8);			//触摸按键初始化
	TFTLCD_Init(BLACK);		//TFTLCD初始化
	TP_Init();				//触摸屏初始化
	SD_Init();				//SD初始化
	W25QXX_Init();			//W25Qxx初始化
	My_RTC_Init();			//RTC初始化
	piclib_init();			//初始化画图
	
	/* FATFS申请内存 */
	exfuns_init();
	
	/* 挂载FLASH SD */
	f_mount(fs[0], "0:", 1);
	f_mount(fs[1], "1:", 1);
		
	/* 获取FLASH总容量和余量 */
	do
	{
		temp++;
 		res=exf_getfree((u8*)"1:",&dtsize,&dfsize);
		delay_ms(200);		   
	}while(res&&temp<20);

	/* 文件系统不存在,格式化FLASH */
	if(res==0X0D)
	{
		LCD_ShowString(5,30,240,16,16, (u8*)"Flash Disk Formatting...");	//格式化FLASH
		res=f_mkfs("1:",1,4096);//格式化FLASH,1,盘符;1,不需要引导区,8个扇区为1个簇
		if(res==0)
		{
			f_setlabel((const TCHAR *)"1:DZXZ");		//设置Flash磁盘的名字为：DZXZ
			LCD_ShowString(5,30+16,240,16,16, (u8*)"OK");//标志格式化成功
 			res=exf_getfree((u8*)"1:",&dtsize,&dfsize);//重新获取容量
		}
	}
	
	/* 检查字库 */
	while(font_init())
	{
		LCD_Clear(WHITE);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		LCD_ShowString(30,50,200,16,16,(u8*)"STM32F407ZET6");
		while(SD_Init())			//检测SD卡
		{
			LCD_ShowString(30,70,200,16,16,(u8*)"SD Card Failed!");
			delay_ms(200);
			LCD_Fill(30,70,200+30,70+16,WHITE);
			delay_ms(200);		    
		}
		LCD_ShowString(30,70,200,16,16,(u8*)"SD Card OK");
		LCD_ShowString(30,90,200,16,16,(u8*)"Font Updating...");
		key=update_font(20,110,16,(u8*)"0:");//更新字库
		while(key)//更新失败		
		{			 		  
			LCD_ShowString(30,110,200,16,16,(u8*)"Font Update Failed!");
			delay_ms(200);
			LCD_Fill(20,110,200+20,110+16,WHITE);
			delay_ms(200);		       
		} 		  
		LCD_ShowString(30,110,200,16,16,(u8*)"Font Update Success!   ");
		delay_ms(1500);	
		LCD_Clear(BLACK);//清屏
	}
	
	/* 打开图片文件夹 */
	res=f_opendir(&picdir,(const TCHAR*)"1:/PICTURE"); 	//打开目录
	if (res==FR_OK)//打开成功
	{
		char i;
		LCD_Clear(BLACK);
		for (i = 0; i < 3; i++)
		{
			ai_load_picfile((const u8*)"1:/PICTURE/Start.gif",0,0,lcddev.width,lcddev.height,1);
		}
		delay_ms(2000);
		LCD_Clear(BLACK);
		ai_load_picfile((const u8*)"1:/PICTURE/LOGO.jpg",0,0,lcddev.width,lcddev.height,1);//显示图片
		delay_ms(2000);
		delay_ms(2000);
	}
	
	//检测SD卡是否存在
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
	Show_Str(20, 5, 240, 24, (u8*)"STM32F407测试程序", 24, 0);
	POINT_COLOR=BLUE;
	LCD_printf(20, 35, "FLASH磁盘总容量:%dKB", dtsize);
	LCD_printf(20, 55, "FLASH磁盘剩余量:%dKB", dfsize);
	
	/* 蓝牙模块检测 */
	if (HC05_Init())
	{
//		LCD_Clear(BACK_COLOR);		   	//清屏
 		POINT_COLOR=RED;			//设置字体为红色	   	   	  
		Show_Str(20, 75, 200, 16, (u8*)"蓝牙不存在或已损坏!", 16, 0);
	}
	else
	{
		Show_Str(20, 75, 200, 16, (u8*)"蓝牙模块正常        ", 16, 0);
	}

	POINT_COLOR = RED;
	Show_Str(20, 100, 200, 32, (u8*)"时钟测试", 32, 0);
	Show_Str(20, 220, 200, 40, (u8*)"电子小组", 40, 0);
	POINT_COLOR=BLUE;
	while (1U)
	{
		/* 判断触摸按键是否按下 */
		/* 成功捕获到了一次上升沿(此函数执行时间至少15ms) */
		if (TPAD_Scan(0))
		{
			LCD_Clear(BACK_COLOR);
			LCD_printf(30, 170, "USB连接...  ");
			USBD_Init(&USB_OTG_dev,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
			delay_ms(1800);
			while (1)
			{
				delay_ms(1);
				if (USB_STA != USB_STATUS_REG)
				{
					LCD_Fill(30,190,240,190+16,BACK_COLOR);//清除显示			  	   
					if(USB_STATUS_REG&0x01)//正在写		  
					{
						LED_On(1);
						LCD_printf(30,190,"USB写入...  ");//提示USB正在写入数据	 
					}
					if(USB_STATUS_REG&0x02)//正在读
					{
						LED_On(1);
						LCD_printf(30,190,"USB读取...  ");//提示USB正在读出数据  		 
					}	 										  
					if(USB_STATUS_REG&0x04)LCD_printf(30,210,"USB写入错误 ");//提示写入错误
					else LCD_Fill(30,210,240,210+16,BACK_COLOR);//清除显示	  
					if(USB_STATUS_REG&0x08)LCD_printf(30,230,"USB读取错误 ");//提示读出错误
					else LCD_Fill(30,230,240,230+16,BACK_COLOR);//清除显示    
					USB_STA=USB_STATUS_REG;//记录最后的状态
				}
				if(Divece_STA!=bDeviceState) 
				{
					if(bDeviceState==1)LCD_printf(30,170,"USB连接      ");//提示USB连接已经建立
					else 
					{
						LCD_printf(30,170,"USB断开连接 ");//提示USB被拔出了
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
						offline_cnt=0;//USB连接了,则清除offline计数器
						bDeviceState=1;
					}else//没有得到轮询 
					{
						offline_cnt++;  
						if(offline_cnt>10)bDeviceState=0;//2s内没收到在线标记,代表USB被拔出了
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
		if((t%10)==0)	//每100ms更新一次显示数据
		{
			RTC_GetTime(RTC_Format_BIN,&RTC_TimeStruct);
			
			sprintf((char*)tbuf,"时间:%02d:%02d:%02d",RTC_TimeStruct.RTC_Hours,RTC_TimeStruct.RTC_Minutes,RTC_TimeStruct.RTC_Seconds); 
			Show_Str(20,140,210,16,tbuf,16,0);	
			
			RTC_GetDate(RTC_Format_BIN, &RTC_DateStruct);
			
			sprintf((char*)tbuf,"日期:20%02d-%02d-%02d",RTC_DateStruct.RTC_Year,RTC_DateStruct.RTC_Month,RTC_DateStruct.RTC_Date); 
			Show_Str(20,160,210,16,tbuf,16,0);	
			sprintf((char*)tbuf,"星期:%d",RTC_DateStruct.RTC_WeekDay); 
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
