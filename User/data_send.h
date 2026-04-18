#ifndef DATA_SEND_H
#define DATA_SEND_H

#include "usart.h"

extern void private_uart_send_data(UART_HandleTypeDef *huart);
extern void private_uart_send_init(void);
extern void private_uart_data_read(void);
extern void warning_oled_display(void);

extern uint8_t is_flag_waring;
extern uint8_t flag_hongwai_warning;
extern uint8_t flag_qingxie_warning;

#endif