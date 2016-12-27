/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   rtc 测试，显示时间格式为: xx:xx:xx
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
	
#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./rtc/bsp_rtc.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./key/bsp_key.h"  
#include "./beep/bsp_beep.h"   


// N = 2^32/365/24/60/60 = 136 年

/*时间结构体，默认时间2000-01-01 00:00:00*/
struct rtc_time systmtime=
{
0,0,0,1,1,2000,0
};

/*时间结构体，闹钟时间2000-01-01 00:00:08*/
struct rtc_time clocktime=
{
8,0,0,1,1,2000,0
};

extern __IO uint32_t TimeDisplay ;


/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main()
{		
	
	uint32_t clock_timestamp;
	uint32_t current_timestamp;
	
//可使用该宏设置是否使用液晶显示
#ifdef  USE_LCD_DISPLAY
	
		ILI9341_Init ();         //LCD 初始化
		LCD_SetFont(&Font8x16);
		LCD_SetColors(RED,BLACK);

		ILI9341_Clear(0,0,LCD_X_LENGTH,LCD_Y_LENGTH);	/* 清屏，显示全黑 */

		ILI9341_DispStringLine_EN(LINE(0),"        BH RTC demo");
#endif
	
	  USART_Config();			
	
		Key_GPIO_Config();
	
		BEEP_GPIO_Config();
		
		/* 配置RTC秒中断优先级 */
	  RTC_NVIC_Config();
	  RTC_CheckAndConfig(&systmtime);
	
		clock_timestamp = mktimev(&clocktime);
	
	  while (1)
	  {
	    /* 每过1s 更新一次时间*/
	    if (TimeDisplay == 1)
	    {
				/* 当前时间 */
				current_timestamp = RTC_GetCounter();
	      Time_Display( current_timestamp,&systmtime); 
				
				//响铃
				if( current_timestamp == clock_timestamp)
				{
					BEEP(ON);
				}
				
	      TimeDisplay = 0;
	    }
			
			//按下按键，通过串口修改时间
			if( Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON  )
			{
				struct rtc_time set_time;

				/*使用串口接收设置的时间，输入数字时注意末尾要加回车*/
				Time_Regulate_Get(&set_time);
				/*用接收到的时间设置RTC*/
				Time_Adjust(&set_time);
				
				//向备份寄存器写入标志
				BKP_WriteBackupRegister(RTC_BKP_DRX, RTC_BKP_DATA);

			} 

			//按下按键，关闭蜂鸣器
			if( Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON  )
			{
				BEEP(OFF);
			}
	  }
}

/***********************************END OF FILE*********************************/

