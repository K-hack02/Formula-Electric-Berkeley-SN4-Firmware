// ********************************** Includes & External ***********************
#include "stm32f4xx_hal.h"
#include <string.h>
#include "FEB_CAN_Library_SN4/gen/feb_can.h"

extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint32_t FEB_CAN_Tx_Mailbox;
extern CAN_HandleTypeDef hcan1; 
static uint8_t heartbeat[8];

// ********************************** Functions *********************************

void FEB_CAN_HEARTBEAT_Init() {
	memset(heartbeat, 0xFF, sizeof(heartbeat));
}

void FEB_CAN_HEARTBEAT_Transmit() {
    (void)FEB_CAN_Send_Std(FEB_CAN_PCU_HEARTBEAT_FRAME_ID, heartbeat, (uint8_t)sizeof(heartbeat));
}

