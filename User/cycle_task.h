#ifndef CYCLE_TASK_H
#define CYCLE_TASK_H

#include "tim.h"

extern volatile uint8_t flag_chassis_task_40ms;
extern volatile uint8_t flag_chassis_behaviour_task;
extern volatile uint8_t flag_data_transmission_task;
extern volatile uint8_t flag_waring_transmission;

#endif // CYCLE_TASK_H