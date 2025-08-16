#ifndef INC_FEB_CAN_DASH_H_
#define INC_FEB_CAN_DASH_H_

// ********************************** Includes ***********************************

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "FEB_CAN_IDs.h"

// ********************************** Functions **********************************

void    FEB_CAN_DASH_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]);
uint8_t FEB_CAN_DASH_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

bool    FEB_CAN_DASH_Get_R2D(void);

#endif /* INC_FEB_CAN_DASH_H_ */