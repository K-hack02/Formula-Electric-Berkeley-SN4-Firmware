// ********************************** Includes & Externs *************************

#include "FEB_Main.h"
#include "FEB_HW.h"
#include "FEB_CAN.h"
#include "FEB_SM.h"
#include "FEB_ADBMS6830B.h"
#include "FEB_CAN_Charger.h"
#include "FEB_CAN_Heartbeat.h"
#include "FEB_UART_Transmit.h"

// ********************************** Functions **********************************

void FEB_Main_Setup() {
	FEB_ADBMS_Init();
	FEB_CAN_Init();
	FEB_CAN_Heartbeat_Init();	
	FEB_CAN_Charger_Init();
	FEB_SM_Init();
	HAL_Delay(5);
}

void FEB_Task_SM() {
	FEB_SM_Process();
}

void FEB_Task_ADBMS() {
	accumulator_t* FEB_ACC = ACC_BeginWrite();
	FEB_ADBMS_Voltage_Process(FEB_ACC);
	FEB_ADBMS_Temperature_Process(FEB_ACC);
	ACC_Publish();
}

void FEB_Task_Balance() {
	FEB_Cell_Balance_Process();
}

void FEB_Task_CAN() {
	FEB_SM_CAN_Transmit();
	FEB_ACC_VOLT_CAN_Transmit();
	FEB_ACC_TEMP_CAN_Transmit();
	FEB_ACC_FAULTS_CAN_Transmit();
}

void FEB_Task_Charging() {
	FEB_CAN_Charger_Process();
}

void FEB_Task_CAN_Charging() {
	FEB_CAN_Charger_transmit();
}

void FEB_Task_UART() {
	// FEB_SM_UART_Transmit();
	// FEB_Transmit_AIR_Status();
	FEB_MONITOR_UART_Transmit();
	FEB_IVT_Serial();
	FEB_CAN_Charger_Serial();
	// FEB_IVT_V1_Transmit();
	// FEB_ADBMS_UART_Transmit();
	// FEB_CAN_Charger_UART_Transmit();

}

void FEB_Task_IVT() {
	FEB_CAN_IVT_Process();
	// FEB_IVT_V1_Transmit();

}
