// ********************************** Includes & Externs ************************

#include "FEB_CAN_PCU.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"

// ********************************** Variables *********************************

typedef struct {
	uint8_t brake_pedal_percent;
	uint8_t inverter_enabled;
} FEB_CAN_PCU_Message_t;

FEB_CAN_PCU_Message_t FEB_CAN_PCU_Message;

// ********************************** Functions *********************************

uint8_t FEB_CAN_PCU_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {

	uint16_t ids[] = {FEB_CAN_BRAKE_FRAME_ID, FEB_CAN_RMS_COMMAND_FRAME_ID};

	for(uint8_t i = 0; i < sizeof(ids) / sizeof(ids[0]); i ++){
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
		filter_config.SlaveStartFilterBank = 14;
		filter_bank++;

		if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
			// Code Error - Shutdown
		}
	}
	return filter_bank;
}

void FEB_CAN_PCU_Str_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]) {

	switch(rx_header->StdId) {
	    case FEB_CAN_BRAKE_FRAME_ID:
	    	FEB_CAN_PCU_Message.brake_pedal_percent = rx_data[0];
	    	break;

	    case FEB_CAN_RMS_COMMAND_FRAME_ID:
	    	FEB_CAN_PCU_Message.inverter_enabled = rx_data[5];
	    	break;
	}
}

uint8_t FEB_CAN_PCU_Get_Brake_Percent(){
	return FEB_CAN_PCU_Message.brake_pedal_percent;
}

uint8_t FEB_CAN_PCU_Get_Inverter_Enabled(){
	return FEB_CAN_PCU_Message.inverter_enabled;
}
