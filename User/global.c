#include <string.h>
#include <math.h>
#include <stdio.h>
#include "stm32f1xx_hal.h"
#include "stdint.h"
#include "global.h"

/*************全局变量*************/

//volatile uint8_t count = 0;
//volatile uint8_t oled_show_flag = 0;

//volatile uint8_t menu_flag_1 = 1; // 一级菜单目录选择变量
//volatile uint8_t menu_flag_2 = 0; // 二级菜单目录选择变量
//volatile uint8_t oled_clear_flag = 0; // 进入下一级菜单标志位


/****************初始化*****************/


/*************中断回调函数*************/

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) // PA4 PA1 PA5 按键中断回调函数 下降沿触发 用于控制OLED菜单
{
	static uint32_t last_ms = 0;
  uint32_t now = HAL_GetTick();
  if (now - last_ms < 100) return;       // 100ms 消抖窗
  last_ms = now;
	
//	uint8_t count_pin = 0; // 仅在二级菜单且只有三个按键的情况过渡
	
  if (GPIO_Pin == GPIO_PIN_4) { // 上一项
		if (menu_state == 1) // 当菜单处于一级菜单时
			{
			  menu_flag_1--;
		    if (menu_flag_1 == 0)
			  	{
			    menu_flag_1 = 8; // 回到第八行
		      }
	    }
		if (menu_state == 2){ // 当菜单处于二级菜单时
			menu_flag_2--;
		  if (menu_flag_2 == 0){
			  menu_flag_2 =4; // 回到第四行
		  }
	  }
  }
	if (GPIO_Pin == GPIO_PIN_1) { // 下一项
    if (menu_state == 1){ // 当菜单处于一级菜单时
			menu_flag_1++;
		  if (menu_flag_1 == 9){
			  menu_flag_1 = 1;
	  	}
		}
		if (menu_state == 2){ // 当菜单处于二级菜单时
			menu_flag_2++;
		  if (menu_flag_2 == 5){
			  menu_flag_2 = 1;
	  	}
		}
  }
	if (GPIO_Pin == GPIO_PIN_5) { // 确认
		// menu_next_flag为奇数+1，为偶数-1
		if (menu_next_flag % 2 == 1) {
			menu_next_flag++; // 进入上一级菜单
		}
		else
		menu_next_flag--; // 进入下一级菜单
	}
}
//	if (GPIO_Pin == GPIO_PIN_5) { // 确认
//		menu_next_flag--; // 进入上一级菜单
//	}



/***************************************/