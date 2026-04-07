#ifndef __MENU_H
#define __MENU_H

#include "OLED.h"
#include "OLED_Data.h"

//extern volatile uint8_t count;

extern volatile uint8_t oled_show_flag; // OLED显示刷新标志位

extern volatile uint8_t menu_flag_1;// 一级菜单目录选择变量 初始时默认为第一行
extern volatile uint8_t menu_flag_2;// 二级菜单目录选择变量 初始时默认为第一行
extern volatile uint8_t menu_next_flag;// 进入下一级菜单标志位 然后进入二级菜单函数
extern volatile uint8_t menu_2_index;// 二级菜单索引 决定进入哪一个二级菜单内容
extern volatile uint8_t oled_clear_flag; // OLED清屏标志位
extern volatile uint8_t menu_1_page; // 一级菜单页面标志位，初始定义为第一页
extern volatile uint8_t menu_2_page; // 二级菜单页面标志位，初始定义为第一页
extern volatile uint8_t menu_state; // 菜单一级二级三级状态标志位

void enter_next_menu(void); // 进入下一级菜单判断函数
void menu_1(void); // 一级菜单
void menu_2(void); // 二级菜单
void menu_tree(void); // 菜单树
void menu_2_content_5(void); // 二级菜单内容5 MPU6050显示
void menu_2_content_1(void); // 二级菜单内容1
void menu_2_content_2(void); // 二级菜单内容2
void menu_reverseArea_1(void); // 一级菜单反显
void menu_reverseArea_2(void); // 二级菜单反显

#endif