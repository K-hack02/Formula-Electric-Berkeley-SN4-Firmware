// **************************************** Includes & External ****************************************

#include "FEB_CAN.h"

extern CAN_HandleTypeDef hcan;

// **************************************** CAN Configuration ****************************************

CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
static CAN_RxHeaderTypeDef FEB_CAN_Rx_Header;

uint8_t FEB_CAN_Tx_Data[8];
uint8_t FEB_CAN_Rx_Data[8];

uint32_t FEB_CAN_Tx_Mailbox;

// **************************************** Functions ****************************************

void FEB_CAN_Init(void) {
	FEB_CAN_Filter_Config();
	if (HAL_CAN_Start(&hcan) != HAL_OK) {
		Error_Handler();
	}
	HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void FEB_CAN_Filter_Config(void) {
	uint8_t filter_bank = 0;
	filter_bank = FEB_CAN_BMS_Filter(&hcan, CAN_RX_FIFO0, filter_bank);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &FEB_CAN_Rx_Header, FEB_CAN_Rx_Data) == HAL_OK) {
		FEB_CAN_BMS_Process_Message(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);
	}
}
