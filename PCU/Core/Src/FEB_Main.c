// ********************************** Includes & External **********************************

#include "FEB_Main.h"

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;



// ********************************** Variables **********************************
char buf[128];
uint8_t buf_len; //stolen from Main_Setup (SN2)



// ********************************** Functions **********************************

void FEB_Main_Setup(void){
	HAL_ADCEx_InjectedStart(&hadc1); //@lovehate - where does this go
//	FEB_Timer_Init();
//	FEB_TPS2482_Setup();
	FEB_CAN_Init(); //FEB_CAN_Init() // The transceiver must be connected otherwise you get sent into an infinite loop
	FEB_CAN_RMS_Setup();
	FEB_CAN_HEARTBEAT_Init();
}

void FEB_Main_While(void){
//	FEB_CAN_ICS_Transmit();
	FEB_SM_ST_t bms_state = FEB_CAN_BMS_getState();

	if (FEB_Ready_To_Drive() && (bms_state == FEB_SM_ST_DRIVE /*|| bms_state == FEB_SM_ST_DRIVE_REGEN*/)) {
		FEB_Normalized_updateAcc();
		FEB_CAN_RMS_Process();
//		FEB_TPS2482_sendReadings();

		HAL_Delay(10);

	} else {
		FEB_Normalized_setAcc0();
		FEB_CAN_RMS_Disable();
	}
//	FEB_Normalized_updateAcc();
//	FEB_CAN_RMS_Process();

	if ((bms_state == "FEB_SM_ST_HEALTH_CHECK") || (FEB_CAN_BMS_getDeviceSelect() == 2)) { // change 0 to pcu device number
		FEB_CAN_HEARTBEAT_Transmit();
	}

	FEB_HECS_update();

	FEB_CAN_RMS_Torque();
	FEB_Normalized_CAN_sendBrake();
//	FEB_CAN_HEARTBEAT_Transmit();

	HAL_Delay(10);
}
