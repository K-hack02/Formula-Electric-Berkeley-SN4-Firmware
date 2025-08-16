#ifndef INC_FEB_CAN_H_
#define INC_FEB_CAN_H_

// ********************************** Includes ***********************************

#include <stdbool.h>
#include "stm32f4xx_hal.h"

// ********************************** Functions **********************************

void     FEB_CAN_Init(void);
void     FEB_CAN_Filter_Config(void);
void     HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan);

bool     FEB_CAN_Send_Std(uint16_t std_id, const uint8_t *data, uint8_t dlc);

void     FEB_SM_CAN_Transmit(void);
void     FEB_CELL_CAN_Transmit(void);
void     FEB_ACC_VOLT_CAN_Transmit(void);
void     FEB_ACC_TEMP_CAN_Transmit(void);
void     FEB_ACC_FAULTS_CAN_Transmit(void);

#endif /* INC_FEB_CAN_H_ */
