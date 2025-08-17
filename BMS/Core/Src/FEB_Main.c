// ********************************** Includes & Externs *************************

#include "FEB_Main.h"
#include "FEB_HW.h"
#include "FEB_CAN.h"
#include "FEB_SM.h"
#include "FEB_CAN_IVT.h"
#include "FEB_ADBMS6830B.h"
#include "FEB_CAN_Charger.h"
#include "FEB_CAN_Heartbeat.h"
#include "FEB_UART_Transmit.h"

// ********************************** Variables **********************************

static int cyc=0;
bool balancing = true;
static int cyc2=0;

// ********************************** Functions **********************************

void FEB_Main_Setup() {

	FEB_ADBMS_Init();
	FEB_CAN_Init();
	FEB_CAN_Heartbeat_Init();	
	FEB_CAN_Charger_Init();
	FEB_SM_Init();
	HAL_Delay(5);
}

void FEB_Task_ADBMS() {
	FEB_ADBMS_Voltage_Process();
	FEB_ADBMS_Temperature_Process();
	HAL_Delay(5);
}

void FEB_Task_SM() {
	FEB_SM_Process();
}

void FEB_Task_Balance() {
	if (++cyc < 4) {
		return;
	}
	cyc = 0;
	FEB_Cell_Balance_Process();
	HAL_Delay(500);
}

void FEB_Task_IVT() {
	FEB_CAN_IVT_Process();
	// FEB_IVT_V1_Transmit();

}

void FEB_Task_CAN() {
	FEB_SM_CAN_Transmit();
	FEB_ACC_VOLT_CAN_Transmit();
	FEB_ACC_TEMP_CAN_Transmit();
	FEB_ACC_FAULTS_CAN_Transmit();
}

void FEB_Task_UART() {
	if(++cyc2<4) {
		return;
	} else {
		cyc2=0;
		// FEB_SM_UART_Transmit();
		// FEB_Transmit_AIR_Status();
		FEB_MONITOR_UART_Transmit();
		FEB_IVT_Serial();
		FEB_CAN_Charger_Serial();
		// FEB_IVT_V1_Transmit();
		// FEB_ADBMS_UART_Transmit();
		// FEB_CAN_Charger_UART_Transmit();
	}
}
