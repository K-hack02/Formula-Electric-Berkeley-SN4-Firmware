#ifndef INC_FEB_CAN_BMS_H_
#define INC_FEB_CAN_BMS_H_

// ********************************** Includes **********************************

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"

// ********************************** Variables **********************************

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

typedef enum {
	FEB_HB_NULL,
	FEB_HB_DASH,
	FEB_HB_PCU,
	FEB_HB_LVPDB,
	FEB_HB_DCU,
	FEB_HB_FSN,
	FEB_HB_RSN
} FEB_HB_t;

// ********************************** Functions **********************************

void FEB_CAN_BMS_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]);
uint8_t FEB_CAN_BMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

FEB_SM_ST_t FEB_CAN_BMS_Get_State(void);
FEB_HB_t 	FEB_CAN_BMS_Get_Ping_Ack(void);
uint16_t 	FEB_CAN_BMS_Get_Pack_Voltage(void);
uint16_t 	FEB_CAN_BMS_Get_Min_Cell_Voltage(void);
float 		FEB_CAN_BMS_Get_Max_Acc_Temp(void);
bool 		FEB_CAN_BMS_GET_FAULTS(void);
bool 		FEB_CAN_GET_IMD_FAULT(void);
bool 		FEB_CAN_BMS_is_stale(void);

#endif /* INC_FEB_CAN_BMS_H_ */
