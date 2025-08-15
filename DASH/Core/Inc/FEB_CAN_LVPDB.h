#ifndef INC_FEB_CAN_LVPDB_H_
#define INC_FEB_CAN_LVPDB_H_

// ********************************** Includes **********************************

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

// ********************************** Functions *********************************

void FEB_CAN_LVPDB_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]);
uint8_t FEB_CAN_LVPDB_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

uint16_t FEB_CAN_LVPDB_Get_BusVoltage(void);

#endif /* INC_FEB_CAN_LVPDB_H_ */
