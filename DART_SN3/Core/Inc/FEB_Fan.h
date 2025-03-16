#ifndef INC_FEB_FAN_H_
#define INC_FEB_FAN_H_

#include <stm32f0xx_hal.h>
#include "FEB_main.h"

void FEB_Fan_All_Speed_Set(uint8_t speed);
void FEB_Fan_1_Speed_Set(uint8_t speed);
void FEB_Fan_2_Speed_Set(uint8_t speed);
void FEB_Fan_3_Speed_Set(uint8_t speed);
void FEB_Fan_4_Speed_Set(uint8_t speed);
void FEB_Fan_5_Speed_Set(uint8_t speed);

void FEB_Fan_PWM_Init(void);
void FEB_Fan_Init(void);
void FEB_Fan_TACH_Init(void);

#endif /* INC_FEB_FAN_H_ */
