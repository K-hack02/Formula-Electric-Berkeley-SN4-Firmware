#ifndef INC_FEB_CAN_AUTO_H_
#define INC_FEB_CAN_AUTO_H_
 
// ********************************** Includes **********************************

#include <stdbool.h>
#include "stm32f4xx_hal.h"

// ********************************** Functions *********************************

uint8_t FEB_CAN_AUTO_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
void FEB_CAN_AUTO_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData);
 
#endif /* INC_FEB_CAN_AUTO_H_ */
 
