// ********************************** Includes & Externs ************************

#include "FEB_CAN_AUTO.h"
#include "FEB_CAN_BMS.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"

// ********************************** Variables *********************************

bool auto_on;
uint16_t torque; 

// ********************************** Functions *********************************

uint8_t FEB_CAN_AUTO_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {0x1F3};

	for (uint8_t i = 0; i < 1; i++) {
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
			//Code Error - shutdown
		}
	}

	return filter_bank;
}

void FEB_CAN_AUTO_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData) {
    switch (pHeader -> StdId){
        case 0x1F3:
        	auto_on = (RxData[0] & 0x01) != 0;
			torque = (RxData[1] << 8) | RxData[2];
            break;
    }
}

