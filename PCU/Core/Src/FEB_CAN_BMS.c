// ********************************** Includes & External **********************************

#include "FEB_CAN_BMS.h"

// *********************************** Struct ************************************

typedef struct BMS_MESSAGE_TYPE {
    uint16_t temp;
    FEB_SM_ST_t status;

} BMS_MESSAGE_TYPE;
BMS_MESSAGE_TYPE BMS_MESSAGE;

// **************************************** Functions ****************************************

uint16_t FEB_CAN_BMS_getTemp(){
	return BMS_MESSAGE.temp;
}



// ***** CAN FUNCTIONS ****

uint8_t FEB_CAN_BMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {FEB_CAN_ID_BMS_STATE,FEB_CAN_ID_BMS_TEMPERATURE};

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

void FEB_CAN_BMS_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData) {
    switch (pHeader -> StdId){
        case FEB_CAN_ID_BMS_TEMPERATURE :
        	BMS_MESSAGE.temp = RxData[2] << 8 | RxData[3];
            break;

        case FEB_CAN_ID_BMS_STATE:
            BMS_MESSAGE.status = RxData[0];
            break;
    }
}

FEB_SM_ST_t FEB_CAN_BMS_getState(){
	return BMS_MESSAGE.status;

}





//// ********************************** Includes & External **********************************
//
//#include "FEB_CAN_BMS.h"
//
//// *********************************** Struct ************************************
//
//
//typedef struct BMS_MESSAGE_TYPE {
//     FEB_SM_ST_t state;
//     uint32_t last_message_time;
//} BMS_MESSAGE_TYPE;
//BMS_MESSAGE_TYPE BMS_MESSAGE;
//
//#define BMS_TIMEOUT 1200
//
//// **************************************** Functions ****************************************
//
//
//
//FEB_SM_ST_t FEB_CAN_BMS_Get_State(){
//	return BMS_MESSAGE.state;
//}
//
//
//// ***** CAN FUNCTIONS ****
//
//uint8_t FEB_CAN_BMS_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
//	uint16_t ids[] = {FEB_CAN_ID_BMS_STATE};
//
//	for (uint8_t i = 0; i < 1; i++) {
//		CAN_FilterTypeDef filter_config;
//
//	    // Standard CAN - 2.0A - 11 bit
//	    filter_config.FilterActivation = CAN_FILTER_ENABLE;
//		filter_config.FilterBank = filter_bank;
//		filter_config.FilterFIFOAssignment = FIFO_assignment;
//		filter_config.FilterIdHigh = ids[i] << 5;
//		filter_config.FilterIdLow = 0;
//		filter_config.FilterMaskIdHigh = 0xFFE0;
//		filter_config.FilterMaskIdLow = 0;
//		filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
//		filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
//		filter_config.SlaveStartFilterBank = 27;
//	    filter_bank++;
//
//		if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
//			//Code Error - shutdown
//		}
//	}
//
//	return filter_bank;
//}
//
//void FEB_CAN_BMS_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData) {
//    switch (pHeader -> StdId){
//        case FEB_CAN_ID_BMS_STATE:
//        	BMS_MESSAGE.state = RxData[0];
//        	if(is_r2d() && !(BMS_MESSAGE.state == FEB_SM_ST_DRIVE || BMS_MESSAGE.state == FEB_SM_ST_DRIVE_REGEN)){
//        		disable_r2d();
//        	}
//        	HAL_GetTick();
//        	break;
//    }
//}
//
//bool FEB_CAN_BMS_is_stale(){
//	if(HAL_GetTick() - BMS_MESSAGE.last_message_time >= BMS_TIMEOUT ){
//		return true;
//	}
//	return false;
//}
//
//
//
//
//
//
