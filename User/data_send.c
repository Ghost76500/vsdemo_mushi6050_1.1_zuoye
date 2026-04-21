#include "data_send.h"
#include "cycle_task.h"
#include <string.h>
#include "mpu6050.h"
#include "OLED.h"

#define ROLL_WARNING_THRESHOLD 20.0f
#define PITCH_WARNING_THRESHOLD 20.0f
#define WARNING_SEND_COOLDOWN_MS 2000U

static uint8_t flag_hongwai_warning = 0; // 红外警告标志位
static uint8_t flag_qingxie_warning = 0; // 倾斜警告标志位
static uint8_t is_flag_waring = 0; // 用于OLED显示的全局变量，表示当前是否有警告需要显示
static uint8_t warning_send_initialized = 0; // 标志位，确保第一次发送警告时不受冷却时间限制
static uint32_t last_warning_send_tick = 0; // 上次发送警告的系统时间戳（单位：毫秒）

static float pitch, roll, yaw;

void private_uart_send_data(UART_HandleTypeDef *huart)
{
  if (flag_waring_transmission)
    {
    uint32_t now = HAL_GetTick();

    if (warning_send_initialized &&
        ((now - last_warning_send_tick) < WARNING_SEND_COOLDOWN_MS)) {
        return;
    }


        uint8_t data_packet[11];

        if (flag_hongwai_warning) {
            // 发送红外警告数据包
            data_packet[3] = 0xAA; // 警告类型 红外
        } else if (flag_qingxie_warning) {
            // 发送倾斜警告数据包
            data_packet[3] = 0xBB; // 警告类型 倾斜
        } else if (flag_hongwai_warning && flag_qingxie_warning) {
            data_packet[3] = 0xCC; // 两个警告
        } else {
            data_packet[3] = 0x00; // 无警告
        } 

        data_packet[5] = 0x50; // 电池电量
        // 构建数据包
        
        data_packet[0] = 0x55; // 包头1
        data_packet[1] = 0xAA; // 包头2
        data_packet[2] = 0x00;
        //data_packet[3] = 0x00; // 警告类型
        data_packet[4] = 0x00;
        //data_packet[5] = 0x00; // 预留数据位
        data_packet[6] = 0x00;
        data_packet[7] = 0x00; // 预留数据位
        data_packet[8] = 0x00;
        data_packet[9] = 0x00;
        data_packet[10] = 0x6B; // 包尾

        // 发送数据包
        HAL_UART_Transmit(huart, data_packet, sizeof(data_packet), HAL_MAX_DELAY);
        last_warning_send_tick = HAL_GetTick();
        warning_send_initialized = 1;

        // 重置警告标志位
        flag_hongwai_warning = 0;
        flag_qingxie_warning = 0;
        flag_waring_transmission = 0;
        is_flag_waring = 0;
        
        //HAL_Delay(2000);
    }
}

void private_uart_data_read(void)
{
    MPU_Get_Pitch_Roll_Yaw(&pitch, &roll, &yaw);
    // 分别检测正值和负值，正常区间为[-30, 30]
    if (pitch > PITCH_WARNING_THRESHOLD || pitch < -PITCH_WARNING_THRESHOLD ||
        roll > ROLL_WARNING_THRESHOLD || roll < -ROLL_WARNING_THRESHOLD) {
        flag_waring_transmission = 1;
        flag_qingxie_warning = 1;
        is_flag_waring = 1;
        return;
    } else if (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13) == GPIO_PIN_RESET) {
        flag_waring_transmission = 1;
        flag_hongwai_warning = 1;
        is_flag_waring = 1;
        //return;
    } else {
        flag_waring_transmission = 0;
        flag_hongwai_warning = 0;
        is_flag_waring = 0;
    }
}

void warning_oled_display(void)
{
    if (is_flag_waring) {
        OLED_ShowString(0, 32, "Warning!", OLED_8X16);
    } else {
        OLED_ShowString(0, 32, "NORMAL", OLED_8X16);
    }
}