#ifndef INC_FEB_CAN_RMS_H_
#define INC_FEB_CAN_RMS_H_

// ********************************** Includes **********************************

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

// ********************************** Functions *********************************

void FEB_CAN_RMS_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]);
uint8_t FEB_CAN_RMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

uint16_t FEB_CAN_RMS_Get_Motor_Speed(void);

#endif /* INC_FEB_CAN_RMS_H_ */
