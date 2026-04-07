#include "data_send.h"
#include "cycle_task.h"
#include <string.h>
#include "mpu6050.h"

#define ROLL_WARNING_THRESHOLD 30.0f
#define PITCH_WARNING_THRESHOLD 30.0f

static uint8_t flag_hongwai_warning = 0;
static uint8_t flag_qingxie_warning = 0;

static float pitch, roll, yaw;

void private_uart_send_data(UART_HandleTypeDef *huart)
{
  if (flag_waring_transmission)
    {
        flag_waring_transmission = 0; // 重置标志位
        
        uint8_t data_packet[11];

        if (flag_hongwai_warning) {
            // 发送红外警告数据包
            data_packet[3] = 0xAA; // 警告类型
        } else {
            data_packet[3] = 0x00; // 无警告
        }
        if (flag_qingxie_warning) {
            // 发送倾斜警告数据包
            data_packet[3] = 0xBB; // 警告类型
        } else {
            data_packet[3] = 0x00; // 无警告
        }
        // 构建数据包
        
        data_packet[0] = 0x55; // 包头1
        data_packet[1] = 0xAA; // 包头2
        data_packet[2] = 0x00;
        //data_packet[3] = 0x00; // 预留数据位
        data_packet[4] = 0x00;
        data_packet[5] = 0x00; // 预留数据位
        data_packet[6] = 0x00;
        data_packet[7] = 0x00; // 预留数据位
        data_packet[8] = 0x00;
        data_packet[9] = 0x00;
        data_packet[10] = 0x6B; // 包尾

        // 发送数据包
        HAL_UART_Transmit(huart, data_packet, sizeof(data_packet), HAL_MAX_DELAY);

        //HAL_Delay(2000);
    }
}

void private_uart_data_read(void)
{
    MPU_Get_Pitch_Roll_Yaw(&pitch, &roll, &yaw);
    // 读取陀螺仪pitch,roll数据，若超过阈值则设置flag_waring_transmission
    if (fabsf(pitch) > PITCH_WARNING_THRESHOLD || fabsf(roll) > ROLL_WARNING_THRESHOLD) {
        flag_waring_transmission = 1;
        flag_hongwai_warning = 1;
    } else {
        flag_waring_transmission = 0;
        flag_hongwai_warning = 0;
    }
}