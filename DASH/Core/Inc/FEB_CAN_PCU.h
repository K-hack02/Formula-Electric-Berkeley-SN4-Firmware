#ifndef INC_FEB_CAN_PCU_H_
#define INC_FEB_CAN_PCU_H_

// ********************************** Includes **********************************

#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

// ********************************** Functions *********************************

void    FEB_CAN_PCU_Str_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]);
uint8_t FEB_CAN_PCU_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

uint8_t FEB_CAN_PCU_Get_Brake_Percent(void);
uint8_t FEB_CAN_PCU_Get_Inverter_Enabled(void);

#endif /* INC_FEB_CAN_PCU_H_ */
