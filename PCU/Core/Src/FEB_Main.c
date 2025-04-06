// ********************************** Includes & External **********************************

#include "FEB_Main.h"

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

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

<<<<<<< HEAD


	if (FEB_Ready_To_Drive() && (bms_state == FEB_SM_ST_DRIVE /*|| bms_state == FEB_SM_ST_DRIVE_REGEN*/)) {
=======
	if (FEB_Ready_To_Drive() && (bms_state == FEB_SM_ST_DRIVE)) {
>>>>>>> 380dd11ef21051f975633da55bd5b08f708a71c5
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
