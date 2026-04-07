#ifndef DATA_SEND_H
#define DATA_SEND_H

#include "usart.h"

extern void private_uart_send_data(UART_HandleTypeDef *huart);
extern void private_uart_send_init(void);
extern void private_uart_data_read(void);

#endif