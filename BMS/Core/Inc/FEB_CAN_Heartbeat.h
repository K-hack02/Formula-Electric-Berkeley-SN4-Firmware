#ifndef INC_FEB_Heartbeat_CAN_H_
#define INC_FEB_Heartbeat_CAN_H_

// ********************************** Includes ***********************************

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "FEB_CAN_IDs.h"

// ********************************** Constants **********************************

#define FEB_FACK_THRESHOLD		20
#define INITIALIZED_THRESHOLD	20

// ********************************** Structs ************************************

typedef enum {
	DEV_IND_IVM,
	DEV_IND_DASH,
	DEV_IND_PCU,
	DEV_IND_DCU,
	DEV_IND_FSN,
	DEV_IND_RSN,
	DEV_IND_LVPDB,
	DEV_IND_DART1,
	DEV_IND_DART2,
	DEV_IND_ALL,
} FEB_DEV_INDEX;

typedef enum {
    INITIALIZED,
    DISCONNECTED,
    CONNECTED,
} FEB_DEV_STATUS;

// ********************************** Functions **********************************

void 		  FEB_CAN_Heartbeat_Init(void);

void          FEB_CAN_Heartbeat_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]);
uint8_t       FEB_CAN_Heartbeat_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

FEB_DEV_STATUS FEB_GetStatus(FEB_DEV_INDEX device_index);
FEB_DEV_STATUS FEB_COMBINED_STATUS(void);

uint64_t FEB_CAN_Heartbeat_Get_Initialized(FEB_DEV_INDEX device_index);
void FEB_CAN_Heartbeat_Decrement_Initialized(FEB_DEV_INDEX device_index);

uint64_t FEB_CAN_Heartbeat_Get_Last_Recieved(FEB_DEV_INDEX device_index);
void FEB_CAN_Heartbeat_Reset_Last_Recieved(FEB_DEV_INDEX device_index);

void FEB_CAN_Heartbeat_Increment_LaOn(FEB_DEV_INDEX device_index);
void FEB_CAN_Heartbeat_Increment_FAck(FEB_DEV_INDEX device_index);

#endif /* INC_FEB_Heartbeat_CAN_H_ */



