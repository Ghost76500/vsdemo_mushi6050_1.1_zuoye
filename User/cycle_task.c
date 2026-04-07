#include "cycle_task.h"
#include <stdint.h>
#include "tim.h"

volatile uint8_t flag_chassis_task_40ms = 0;
volatile uint8_t flag_chassis_behaviour_task = 0;
volatile uint8_t flag_data_transmission_task = 0;
volatile uint8_t flag_waring_transmission = 0;

static void cycle_task_10ms(void);
static void cycle_task_20ms(void);
static void cycle_task_40ms(void);
static void cycle_task_100ms(void);

/* 中断调度周期任务
 * TIM7 1ms  1khz
 * 
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM4) // 如果是 TIM7 触发的中断
    {
        // 在这里添加 TIM7 的中断处理代码
        cycle_task_10ms(); // 调用10ms周期任务（任务状态机）
        cycle_task_20ms(); // 调用20ms周期任务
        cycle_task_40ms(); // 调用40ms周期任务
        cycle_task_100ms(); // 调用100ms周期任务
        //MPU6050_Update_INS_Angle(); // 更新 MPU6050 姿态角缓存（单位：rad）
    }
}

/* 中断调度周期任务
 * 10ms任务
 * 任务状态机运行
 */
static void cycle_task_10ms(void)
{
    static uint8_t counter_10ms = 0;
    counter_10ms++;
    if (counter_10ms >= 10) // 10ms 到达
    {
        counter_10ms = 0;
    } else {
        return; // 未到10ms，直接返回
    }

    /*---------在这里添加10ms周期任务的代码---------*/
    
}

/* 中断调度周期任务
 * 20ms任务
 * 获取encoder_motor_t指针
 */
static void cycle_task_20ms(void)
{
    static uint8_t counter_20ms = 0;
    counter_20ms++;
    if (counter_20ms >= 20) // 20ms 到达
    {
        counter_20ms = 0;
    } else {
        return; // 未到20ms，直接返回
    }

    /*---------在这里添加20ms周期任务的代码---------*/
    

}

/* 中断调度周期任务
 * 40ms任务
 * 
 */
 static void cycle_task_40ms(void)
 {
     static uint8_t counter_40ms = 0;
     counter_40ms++;
     if (counter_40ms >= 40) // 40ms 到达
     {
         counter_40ms = 0;
     } else {
         return; // 未到40ms，直接返回
     }

     /*---------在这里添加40ms周期任务的代码---------*/
     flag_chassis_task_40ms = 1;
     flag_data_transmission_task = 1;
 }

/* 中断调度周期任务
* 100ms任务
* 
*/

static void cycle_task_100ms(void)
{
    static uint8_t counter_100ms = 0;
    counter_100ms++;
    if (counter_100ms >= 100) // 100ms 到达
    {
        counter_100ms = 0;
    } else {
        return; // 未到100ms，直接返回
    }

    /*---------在这里添加100ms周期任务的代码---------*/
    flag_chassis_behaviour_task = 1;
    
    
}


