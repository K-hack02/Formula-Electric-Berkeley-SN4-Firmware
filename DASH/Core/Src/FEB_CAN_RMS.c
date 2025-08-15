// ********************************** Includes & Externs ************************

#include "FEB_CAN_RMS.h"

// ********************************** Local Definitions **************************

#define FEB_CAN_RMS_MOTOR_SPEED_FRAME_ID   (0xA5u)

// ********************************** Variables **********************************

static uint16_t rms_motor_speed = 0;

// ********************************** Functions **********************************

uint8_t FEB_CAN_RMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
    uint16_t ids[] = {FEB_CAN_RMS_MOTOR_SPEED_FRAME_ID};

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
			//Code Error - shutdown
		}
	}

	return filter_bank;
}

void FEB_CAN_RMS_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]) {
    switch (rx_header->StdId) {
        case FEB_CAN_RMS_MOTOR_SPEED_FRAME_ID:
            if (rx_data[3] == 255u) {
                rms_motor_speed = 0u;
            } else {
                rms_motor_speed = (uint16_t)(((uint16_t)rx_data[2] << 8) | rx_data[3]);
            }
            break;
    }
}

uint16_t FEB_CAN_RMS_Get_Motor_Speed(void) {
    return rms_motor_speed;
}
