#ifndef INC_FEB_CAN_BMS_H_
#define INC_FEB_CAN_BMS_H_

// ********************************** Includes **********************************

#include "stm32f4xx_hal.h"
#include <stdio.h>

// ********************************** BMS States ********************************
typedef enum {
	FEB_SM_ST_BOOT,
	FEB_SM_ST_LV,
	FEB_SM_ST_HEALTH_CHECK,
	FEB_SM_ST_PRECHARGE,
	FEB_SM_ST_ENERGIZED,
	FEB_SM_ST_DRIVE,
	FEB_SM_ST_FREE,
	FEB_SM_ST_CHARGER_PRECHARGE,
	FEB_SM_ST_CHARGING,
	FEB_SM_ST_BALANCE,
	FEB_SM_ST_FAULT_BMS,
	FEB_SM_ST_FAULT_BSPD,
	FEB_SM_ST_FAULT_IMD,
	FEB_SM_ST_FAULT_CHARGING,
	FEB_SM_ST_DEFAULT
} FEB_SM_ST_t;

// ********************************** Functions *********************************

uint16_t FEB_CAN_BMS_getTemp();
uint8_t FEB_CAN_BMS_getDeviceSelect();
FEB_SM_ST_t FEB_CAN_BMS_getState();

void FEB_CAN_BMS_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData);
uint8_t FEB_CAN_BMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

#endif /* INC_FEB_CAN_BMS_H_ */
