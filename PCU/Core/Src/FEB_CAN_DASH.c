// ********************************** Includes and Externs **********************

#include "FEB_CAN_DASH.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"

extern CAN_HandleTypeDef hcan1;
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint8_t FEB_CAN_Tx_Data[8];
extern uint32_t FEB_CAN_Tx_Mailbox;

// ********************************** Variables *********************************

static bool ready_to_drive = false;

// ********************************** Functions *********************************

uint8_t FEB_CAN_DASH_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {FEB_CAN_DASH_IO_FRAME_ID};

	for (uint8_t i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
		CAN_FilterTypeDef filter_config;

	    // Standard CAN - 2.0A - 11 bit
	    filter_config.FilterActivation = CAN_FILTER_ENABLE;
		filter_config.FilterBank = filter_bank;
		filter_config.FilterFIFOAssignment = FIFO_assignment;
		filter_config.FilterIdHigh = ids[i] << 5;
		filter_config.FilterIdLow = 0;
		filter_config.FilterMaskIdHigh = 0xFFE0;
		filter_config.FilterMaskIdLow = 0;
		filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
		filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
		filter_config.SlaveStartFilterBank = 27;
	    filter_bank++;

		if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
			// Code Error - Shutdown
		}
	}

	return filter_bank;
}

void FEB_CAN_DASH_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]) {
	switch(rx_header->StdId) {
		case FEB_CAN_DASH_IO_FRAME_ID:

			ready_to_drive = ((rx_data[0] & 0b00000010) != 0);
			break;
	}
}

bool FEB_Ready_To_Drive() {
	return ready_to_drive;
}
