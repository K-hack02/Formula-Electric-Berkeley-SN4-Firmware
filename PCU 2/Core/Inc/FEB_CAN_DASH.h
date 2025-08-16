#ifndef INC_FEB_CAN_DASH_H_
#define INC_FEB_CAN_DASH_H_
// ********************************** Includes **********************************

#include <stdbool.h>
#include "stm32f4xx_hal.h"

// ********************************** Functions *********************************

void FEB_CAN_DASH_Store_Msg(CAN_RxHeaderTypeDef *FEB_CAN_Rx_Header, uint8_t FEB_CAN_Rx_Data[]);
uint8_t FEB_CAN_DASH_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

bool FEB_Ready_To_Drive();

#endif /* INC_FEB_CAN_DASH_H_ */
