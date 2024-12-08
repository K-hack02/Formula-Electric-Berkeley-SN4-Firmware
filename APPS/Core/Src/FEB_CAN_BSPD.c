// ********************************** Includes & External **********************************

#include "FEB_CAN_BSPD.h"

extern CAN_HandleTypeDef hcan1;
extern uint8_t FEB_CAN_Tx_Data[8];
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint32_t FEB_CAN_Tx_Mailbox;
// *********************************** Variables ************************************


struct {
  uint8_t state;
} BSPD;

// ********************************** Functions **********************************

//Reads the status of the BSPD if the BSPD reset is active.
void FEB_CAN_BSPD_checkReset(){
	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2)){
		BSPD.state = 1;
	} else {
		BSPD.state = 0;
	}
	FEB_CAN_BSPD_Transmit();

}

void FEB_CAN_BSPD_Transmit(void) {
	// Initialize Transmission Header
    // Write Code Here
	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 1;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_ID_APPS_BSPD; //ID for sending paramater messages for RMS
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = BSPD.state;


	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}
}
