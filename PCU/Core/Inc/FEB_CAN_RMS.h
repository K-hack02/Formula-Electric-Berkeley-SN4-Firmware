
#ifndef INC_FEB_CAN_RMS_H_
#define INC_FEB_CAN_RMS_H_

// ********************************** Includes **********************************

#include <stdint.h>
#include "stm32f4xx_hal.h"

// ********************************** Functions *********************************

void     FEB_CAN_RMS_Setup(void);
void     FEB_CAN_RMS_Process(void);
void     FEB_CAN_RMS_Disable(void);


void     FEB_CAN_RMS_Torque(void);
void     FEB_CAN_RMS_AUTO_Torque(uint16_t torque);
void     FEB_CAN_RMS_Transmit_updateTorque(void);


uint8_t  FEB_CAN_RMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
void     FEB_CAN_RMS_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData);

float    FEB_CAN_RMS_getMaxTorque(void);

#endif /* INC_FEB_CAN_RMS_H_ */
