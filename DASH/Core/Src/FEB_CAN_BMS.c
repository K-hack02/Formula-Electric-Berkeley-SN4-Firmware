// ********************************** Includes & Externs ************************

#include "FEB_CAN_BMS.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"

// ********************************** Variables *********************************

typedef struct BMS_MESSAGE_TYPE {
	FEB_SM_ST_t 	bms_state;
	FEB_HB_t 		ping_ack;
    uint32_t 		last_message_time;
    uint16_t 		pack_voltage;
	uint16_t		min_cell_voltage;
	float 			max_cell_temp;
	bool 			bms_fault;
	bool 			imd_fault;
} BMS_MESSAGE_TYPE;

BMS_MESSAGE_TYPE bms_message;

#define BMS_TIMEOUT 1200

// ********************************** Functions *********************************

uint8_t FEB_CAN_BMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {FEB_CAN_BMS_STATE_FRAME_ID, FEB_CAN_BMS_ACCUMULATOR_VOLTAGE_FRAME_ID, 
		FEB_CAN_BMS_ACCUMULATOR_TEMPERATURE_FRAME_ID, FEB_CAN_ACCUMULATOR_FAULTS_FRAME_ID};

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

void FEB_CAN_BMS_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]) {
    switch (rx_header -> StdId){
        case FEB_CAN_BMS_STATE_FRAME_ID:
        	bms_message.bms_state = rx_data[0] & 0x1F;
			bms_message.ping_ack = (rx_data[0] & 0xE0) >> 5;
        	bms_message.last_message_time = HAL_GetTick();
        	break;

		case FEB_CAN_BMS_ACCUMULATOR_VOLTAGE_FRAME_ID:
			bms_message.pack_voltage = (rx_data[1] << 8) | rx_data[0];
			bms_message.min_cell_voltage = (rx_data[3] << 8) | rx_data[2];
			break;

		case FEB_CAN_BMS_ACCUMULATOR_TEMPERATURE_FRAME_ID:
			bms_message.max_cell_temp = (rx_data[5] << 8) | rx_data[4];
			break;

		case FEB_CAN_ACCUMULATOR_FAULTS_FRAME_ID:
		        	bms_message.bms_fault = (rx_data[0] & 0x01) == 0x01;
		        	bms_message.imd_fault = (rx_data[0] & 0x02) == 0x02;
    }
}

FEB_SM_ST_t FEB_CAN_BMS_Get_State(){
	return bms_message.bms_state;
}

FEB_HB_t FEB_CAN_BMS_Get_Ping_Ack(){
	return bms_message.ping_ack;
}

uint16_t FEB_CAN_BMS_Get_Pack_Voltage(){
	return bms_message.pack_voltage;
}

uint16_t FEB_CAN_BMS_Get_Min_Cell_Voltage(){
	return bms_message.min_cell_voltage;
}

float FEB_CAN_BMS_Get_Max_Acc_Temp(){
	return bms_message.max_cell_temp;
}

bool FEB_CAN_BMS_GET_FAULTS(){
	return (bms_message.bms_fault || bms_message.imd_fault);
}

bool FEB_CAN_GET_IMD_FAULT(){
	return bms_message.imd_fault;
}

bool FEB_CAN_BMS_is_stale(){
	return (HAL_GetTick() - bms_message.last_message_time) >= BMS_TIMEOUT;
}
