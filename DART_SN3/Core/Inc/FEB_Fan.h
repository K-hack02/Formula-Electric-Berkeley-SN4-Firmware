#ifndef INC_FEB_FAN_H_
#define INC_FEB_FAN_H_

// ********************************** Includes & External **********************************

#include <stm32f0xx_hal.h>
#include <stdbool.h>

// ********************************** Defines **********************************

#define TACH_FILTER_EXPONENT 2

#define TIMCLOCK   	48000000
#define PRESCALAR  	1
#define REF_CLOCK	TIMCLOCK / PRESCALAR
#define NUM_FANS	5

#define ADC_FILTER_EXPONENT 2

// ********************************** Function Prototypes **********************************

void FEB_Fan_Init(void);

void FEB_Fan_CAN_Msg_Process(uint8_t *FEB_CAN_Rx_Data);

void FEB_Fan_PWM_Init(void);
void FEB_Fan_All_Speed_Set(uint8_t speed);

void FEB_Fan_TACH_Init(void);
void FEB_Fan_TACH_Callback(TIM_HandleTypeDef *htim);

#endif /* INC_FEB_FAN_H_ */
