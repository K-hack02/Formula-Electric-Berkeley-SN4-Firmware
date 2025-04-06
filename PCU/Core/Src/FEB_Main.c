// ********************************** Includes & External **********************************

#include "FEB_Main.h"

extern ADC_HandleTypeDef hadc1;

// ********************************** Variables **********************************
char buf[128];
uint8_t buf_len;

// ********************************** Functions **********************************

void FEB_Main_Setup(void){
	HAL_ADCEx_InjectedStart(&hadc1);
	FEB_CAN_Init();
	FEB_CAN_RMS_Setup();
}

void FEB_Main_While(void){
	FEB_SM_ST_t bms_state = FEB_CAN_BMS_getState();

	if (FEB_Ready_To_Drive() && (bms_state == FEB_SM_ST_DRIVE)) {
		FEB_Normalized_updateAcc();
		FEB_CAN_RMS_Process();

	} else {
		FEB_Normalized_setAcc0();
		FEB_CAN_RMS_Disable();
	}

	FEB_HECS_update();

	FEB_CAN_RMS_Torque();
	FEB_Normalized_CAN_sendBrake();

	HAL_Delay(10);
}
