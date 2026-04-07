#include "OLED.h"
#include "OLED_Data.h"
#include <string.h>
#include <math.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "menu.h"
#include "mpu6050.h"

volatile uint8_t menu_state = 1; // 菜单一级二级状态标志位
volatile uint8_t menu_flag_1 = 1; // 一级菜单目录选择变量 初始时默认为第一行
volatile uint8_t menu_1_page = 1; // 一级菜单页面标志位，初始定义为第一页
volatile uint8_t menu_flag_2 = 1; // 二级菜单目录选择变量 初始时默认为第一行
volatile uint8_t menu_2_page = 1; // 二级菜单页面标志位，初始定义为第一页
volatile uint8_t menu_next_flag = 1; // 进入下一级菜单标志位 然后进入二级菜单函数
volatile uint8_t menu_2_index = 0; // 二级菜单索引 决定进入哪一个二级菜单内容
volatile uint8_t oled_show_flag = 0; // OLED显示函数标志位，决定屏幕的刷新，在TIM2中被修改,200ms

void menu_tree(void)
{
	if (oled_show_flag == 0) {return; }// 未到刷新时间则直接返回

	if (menu_state == 1){menu_1();} else if (menu_state == 2){menu_2();}
	enter_next_menu(); // 是否进入下一级菜单的判断函数
	OLED_Update();
	OLED_Clear();
	oled_show_flag = 0; // 清除刷新标志位
}

void menu_1(void) // 一级菜单内容 页面节点 
{
	if (menu_flag_1 >= 1 && menu_flag_1 <= 4){ // 这里改目录 
	  OLED_ShowChinese(0, 0,  "状态显示");
	  OLED_ShowChinese(0, 16, "控制模式");
	  OLED_ShowChinese(0, 32, "速度调整");
	  OLED_ShowString(0, 48, "MPU6050", OLED_8X16);
		menu_reverseArea_1();
	} else if (menu_flag_1 >= 5 && menu_flag_1 <= 8){ // 这里改目录
		OLED_ShowChinese(0, 0,  "运行日志");
		OLED_ShowChinese(0, 16, "未设置");
		OLED_ShowChinese(0, 32, "未设置");
		OLED_ShowChinese(0, 48, "版本信息");
		menu_reverseArea_1();
	}
}


void menu_2(void) // 二级菜单内容 页面节点 
{
	switch(menu_2_index)
	{
			case 1:
				menu_2_content_1(); // 状态显示 
			break;
			case 2:
				menu_2_content_2();
			break;
			case 3:
				OLED_ShowChinese(0, 0, "前方的区域以后再");
			  OLED_ShowChinese(0, 16, "来探索吧");
		  	OLED_ShowString(64, 16, "~          ", OLED_8X16);
			break;
			case 4: // MPU6050
			  menu_2_content_5();
			break;
			case 5: // MPU6050
				OLED_ShowChinese(0, 0, "前方的区域以后再");
			  OLED_ShowChinese(0, 16, "来探索吧");
		  	OLED_ShowString(64, 16, "~          ", OLED_8X16);
			break;
			case 6:
				OLED_ShowChinese(0, 0, "前方的区域以后再");
			  OLED_ShowChinese(0, 16, "来探索吧");
		  	OLED_ShowString(64, 16, "~          ", OLED_8X16);
			break;
			case 7:
				OLED_ShowChinese(0, 0, "前方的区域以后再");
			  OLED_ShowChinese(0, 16, "来探索吧");
			  OLED_ShowString(64, 16, "~          ", OLED_8X16);
			break;
			case 8: // 版本号
				OLED_ShowString(0, 0, "vision 0.1.0", OLED_8X16);
			break;
	}
}


void enter_next_menu(void) // 进入下一级菜单判断函数 
{
	if (menu_next_flag == 1){
		menu_state = 1;
	} else if (menu_next_flag == 2){
		menu_2_index = menu_flag_1; // 二级菜单索引赋值
		menu_state = 2;
	} else if (menu_next_flag == 3){
		menu_state = 3;
	} else if (menu_next_flag > 3){
		menu_next_flag = 3;
	}
}

void menu_2_content_1(void) // 状态显示 
{
  OLED_ShowChinese(0, 0, "控制模式未选择");
	OLED_ShowChinese(0, 16, "速度"); OLED_ShowString(36, 16, "2.3 rad/s", OLED_8X16);
	OLED_ShowChinese(0, 32, "正常运行中");
}

void menu_2_content_2(void) // 控制模式
{
  OLED_ShowChinese(0, 0, "手动模式");
	OLED_ShowChinese(0, 16, "自动模式");
	menu_reverseArea_2();
}


void menu_2_content_5(void) // 显示MPU6050 以及建议
{
	
}

void menu_reverseArea_1(void)
{
	switch(menu_flag_1)
		{
			case 1:
				OLED_ReverseArea(0, 0, 128, 16);
			break;
			case 2:
				OLED_ReverseArea(0, 16, 128, 16);
			break;
			case 3:
				OLED_ReverseArea(0, 32, 128, 16);
			break;
			case 4:
				OLED_ReverseArea(0, 48, 128, 16);
			break;
	    case 5:
				OLED_ReverseArea(0, 0, 128, 16);
			break;
			case 6:
				OLED_ReverseArea(0, 16, 128, 16);
			break;
			case 7:
				OLED_ReverseArea(0, 32, 128, 16);
			break;
			case 8:
				OLED_ReverseArea(0, 48, 128, 16);
			break;
	  }
}

void menu_reverseArea_2(void)
{
	switch(menu_flag_2)
		{
			case 1:
				OLED_ReverseArea(0, 0, 128, 16);
			break;
			case 2:
				OLED_ReverseArea(0, 16, 128, 16);
			break;
			case 3:
				OLED_ReverseArea(0, 32, 128, 16);
			break;
			case 4:
				OLED_ReverseArea(0, 48, 128, 16);
			break;
//	    case 5:
//				OLED_ReverseArea(0, 0, 128, 16);
//			break;
//			case 6:
//				OLED_ReverseArea(0, 16, 128, 16);
//			break;
//			case 7:
//				OLED_ReverseArea(0, 32, 128, 16);
//			break;
//			case 8:
//				OLED_ReverseArea(0, 48, 128, 16);
//			break;
	  }
}