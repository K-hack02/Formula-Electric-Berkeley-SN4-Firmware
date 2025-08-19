// ********************************** Includes & Externs *************************

#include "FEB_SM.h"
#include "FEB_HW.h"
#include "FEB_CAN.h"
#include "FEB_CAN_IVT.h"
#include "FEB_CAN_DASH.h"
#include "FEB_ADBMS6830B.h"
#include "FEB_CAN_Charger.h"
#include "FEB_CAN_Heartbeat.h"

#include <stdatomic.h>
#include "stm32f4xx_hal.h"

extern osMessageQueueId_t Charging_ControlHandle;
extern osMessageQueueId_t Balance_ControlHandle;

// ********************************** Variables **********************************

static _Atomic(FEB_SM_ST_t) SM_Current_State;

// ********************************** Transition Functions ***********************

static void bootTransition(FEB_SM_ST_t);
static void LVPowerTransition(FEB_SM_ST_t);
static void HealthCheckTransition(FEB_SM_ST_t);
static void PrechargeTransition(FEB_SM_ST_t);
static void EnergizedTransition(FEB_SM_ST_t);
static void DriveTransition(FEB_SM_ST_t);
static void FreeTransition(FEB_SM_ST_t);
static void ChargingTransition(FEB_SM_ST_t);
static void BalanceTransition(FEB_SM_ST_t);
static void BMSFaultTransition(FEB_SM_ST_t);
static void BSPDFaultTransition(FEB_SM_ST_t);
static void IMDFaultTransition(FEB_SM_ST_t);
static void ChargingFaultTransition(FEB_SM_ST_t);
static void ChargingPrechargeTransition(FEB_SM_ST_t next_state);

static void (*transitionVector[14])(FEB_SM_ST_t)={
		bootTransition,
		LVPowerTransition,
		HealthCheckTransition,
		PrechargeTransition,
		EnergizedTransition,
		DriveTransition,
		FreeTransition,
		ChargingPrechargeTransition,
		ChargingTransition,
		BalanceTransition,
		BMSFaultTransition,
		BSPDFaultTransition,
		IMDFaultTransition,
		ChargingFaultTransition
};

static void transition(FEB_SM_ST_t next_state) {
	(transitionVector[SM_Current_State])(next_state);
	FEB_Config_Update(SM_Current_State);
}

// ********************************** Functions **********************************

void FEB_SM_Init(void) {
	SM_Current_State=FEB_SM_ST_BOOT;
	
	// Keep air+ and precharge relays open
	FEB_PIN_RST(PN_PC_AIR);
	FEB_PIN_RST(PN_PC_REL);

	// Set BMS Shutdown Circuit
	FEB_PIN_SET(PN_BMS_SHUTDOWN);

	// Turn off Fault Indicator
	FEB_PIN_RST(PN_BMS_IND);

	FEB_SM_Transition(FEB_SM_ST_LV);
}

FEB_SM_ST_t FEB_SM_Get_Current_State(void) {
    return atomic_load_explicit(&SM_Current_State, memory_order_acquire);
}

void FEB_SM_Transition(FEB_SM_ST_t next_state) {
	transition(next_state);
}

void FEB_SM_Process(void) {
	transitionVector[FEB_SM_Get_Current_State()](FEB_SM_ST_DEFAULT);
}

static void fault(FEB_SM_ST_t FAULT_TYPE) {
	SM_Current_State = FAULT_TYPE;
	if (FEB_ADBMS_Get_Error_Type() == 0) {
		FEB_ADBMS_Update_Error_Type(0x40);
	}

	// Open Shutdown Circuit 
	FEB_PIN_RST(PN_BMS_SHUTDOWN);

	// Turn off Fault Indicator
	FEB_PIN_SET(PN_INDICATOR);

	//Delay before resetting signals
	RTOS_DELAY_MS(500);

	// Open air+ and precharge relays for redundancy
	FEB_PIN_RST(PN_PC_AIR); 
	FEB_PIN_RST(PN_PC_REL);
}

static FEB_SM_ST_t updateStateProtected(FEB_SM_ST_t next_state){
	if(SM_Current_State==FEB_SM_ST_FAULT_BMS)
		return FEB_SM_ST_FAULT_BMS;
	SM_Current_State=next_state;
	return next_state;
}

static void bootTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_LV:
		updateStateProtected(FEB_SM_ST_LV);
		break;
	case FEB_SM_ST_DEFAULT:
		// If air+ and precharge relays open
		if(FEB_PIN_RD(PN_AIRP_SENSE) == FEB_RELAY_STATE_OPEN && FEB_PIN_RD(PN_PC_REL) == FEB_RELAY_STATE_OPEN){
			bootTransition(FEB_SM_ST_LV);
		}
		break;
	default:
		return;
	}

}

static void LVPowerTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_HEALTH_CHECK:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_FREE:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_DEFAULT:

		// If Shutdown Circuit(air-) is closed
		if (FEB_PIN_RD(PN_SHS_IN) == FEB_RELAY_STATE_CLOSE) {
			LVPowerTransition(FEB_SM_ST_HEALTH_CHECK);
			break;
		}

		FEB_DEV_STATUS accum_status = FEB_COMBINED_STATUS();
		if (accum_status == DISCONNECTED) {
			LVPowerTransition(FEB_SM_ST_FREE);
			break;
		} 

		break;

	default:
		return;
	}
}

static void HealthCheckTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_LV:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_PRECHARGE:
		// Precharge relay now closed, but air+ should still be open
		FEB_PIN_RST(PN_PC_AIR);
		FEB_PIN_SET(PN_PC_REL);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_DEFAULT:
		// If Shutdown Circuit(air-) is open
		if(FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN) {
					HealthCheckTransition(FEB_SM_ST_LV);
		}
		
		FEB_DEV_STATUS accum_status = FEB_COMBINED_STATUS();
		if (accum_status == DISCONNECTED) {
			LVPowerTransition(FEB_SM_ST_FREE);
			break;
		} else if (accum_status == CONNECTED) {
			// If air- is closed, air+ is open, precharge is open
			if(FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_CLOSE && 
					FEB_PIN_RD(PN_AIRP_SENSE) == FEB_RELAY_STATE_OPEN &&
					FEB_PIN_RD(PN_PC_REL) == FEB_RELAY_STATE_OPEN) 
				{
				HealthCheckTransition(FEB_SM_ST_PRECHARGE);
			}
		}

		break;

	default:
		return;
	}
}

static void PrechargeTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_LV:
	case FEB_SM_ST_HEALTH_CHECK:
		// Open air+ and precharge relays
		FEB_PIN_RST(PN_PC_AIR);
		FEB_PIN_RST(PN_PC_REL);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_ENERGIZED:
		// Close air- and open precharge
		FEB_PIN_SET(PN_PC_AIR);
		RTOS_DELAY_MS(500);
		FEB_PIN_RST(PN_PC_REL);
		updateStateProtected(FEB_SM_ST_ENERGIZED);
		break;

	case FEB_SM_ST_DEFAULT:
		// If Shutdown Circuit (air-) is open
		if (FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN || FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_OPEN ) {
			PrechargeTransition(FEB_SM_ST_LV);
		}

		//Keep air+ open for redundancy
		FEB_PIN_RST(PN_PC_AIR);

		//Hold precharge relay closed for redundancy
		FEB_PIN_SET(PN_PC_REL);

		if (FEB_ADBMS_Precharge_Complete()) {
			PrechargeTransition(FEB_SM_ST_ENERGIZED);
		}

		break;

	default:
		return;
	}


}

static void EnergizedTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_DRIVE:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_LV:
	case FEB_SM_ST_HEALTH_CHECK:
		// Open air+ and precharge relays
		FEB_PIN_RST(PN_PC_AIR);
		FEB_PIN_RST(PN_PC_REL);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_DEFAULT:
		// If Shutdown Circuit (air-) is open
		if (FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN || FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_OPEN ) {
			PrechargeTransition(FEB_SM_ST_LV);
		}

		// If DASH sends R2D
		if(FEB_CAN_DASH_Get_R2D()){
			EnergizedTransition(FEB_SM_ST_DRIVE);
		}

		break;

	default:
		return;
	}

}

static void DriveTransition(FEB_SM_ST_t next_state){
	switch(next_state){
	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_BSPD:
		fault(next_state);
		break;

	case FEB_SM_ST_LV:
	case FEB_SM_ST_HEALTH_CHECK:
		// Open air+ and precharge relays
		FEB_PIN_RST(PN_PC_AIR);
		FEB_PIN_RST(PN_PC_REL);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_ENERGIZED:
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_DEFAULT:
		// If Shutdown Circuit (air-) is open
		if (FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN || FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_OPEN ) {
			PrechargeTransition(FEB_SM_ST_LV);
		}

		// If DASH revokes R2D
		if(!FEB_CAN_DASH_Get_R2D()){
			DriveTransition(FEB_SM_ST_ENERGIZED);
		}

		break;

	default:
		return;
	}
}

static void FreeTransition(FEB_SM_ST_t next_state){
	switch(next_state){

	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
		fault(FEB_SM_ST_FAULT_CHARGING);
		break;

	case FEB_SM_ST_FREE:
	case FEB_SM_ST_LV:
		// Open air+ and precharge relays
		FEB_PIN_RST(PN_PC_AIR);
		FEB_PIN_RST(PN_PC_REL);
		updateStateProtected(next_state);
		break;

	case FEB_SM_ST_CHARGER_PRECHARGE:
		// Precharge relay now closed, but air+ should still be open
		FEB_PIN_RST(PN_PC_AIR);
		FEB_PIN_SET(PN_PC_REL);
		updateStateProtected(next_state);
		break;
	case FEB_SM_ST_BALANCE:
		{
			uint8_t control = BAL_CTRL_START;
			(void)osMessageQueuePut(Balance_ControlHandle, &control, 0, 0);
		}
		updateStateProtected(next_state);
		break;
	case FEB_SM_ST_DEFAULT:

		FEB_DEV_STATUS accum_status = FEB_COMBINED_STATUS();
		if (accum_status == CONNECTED) {
			LVPowerTransition(FEB_SM_ST_LV);
		}

		bool charging_status = FEB_CAN_Charging_Status();
		GPIO_PinState charge_sense = HAL_GPIO_ReadPin(PN_CHARGE_SENSE.group, PN_CHARGE_SENSE.pin);

		// If Shutdown Circuit(air-) is closed, charge sense triggered, accumulator status is within bounds, and charger is on and connected
		if (FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_CLOSE && charge_sense == GPIO_PIN_SET && charging_status && FEB_CAN_Charger_Received()){
				FreeTransition(FEB_SM_ST_CHARGER_PRECHARGE);
		}
		
		bool balancing_status = FEB_Cell_Balancing_Status();
		GPIO_PinState balance_sense = false;

		// If Shutdown Circuit(air-) is closed, balance sense triggered, accumulator status is within bounds
		if (FEB_PIN_RD(PN_AIRM_SENSE) == FEB_RELAY_STATE_CLOSE && balance_sense && balancing_status){
				FreeTransition(FEB_SM_ST_BALANCE);
		}


	default:
		return;
	}

}

static void ChargingPrechargeTransition(FEB_SM_ST_t next_state) {
	switch(next_state) {

	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
		fault(FEB_SM_ST_FAULT_CHARGING);
		break;
	case FEB_SM_ST_FREE:
		// Open air+ and precharge relays
		FEB_PIN_RST(PN_PC_AIR);
		FEB_PIN_RST(PN_PC_REL);
	case FEB_SM_ST_LV:
		break;
	case FEB_SM_ST_CHARGING:
		//Close air+ and open precharge
		FEB_PIN_SET(PN_PC_AIR);
		RTOS_DELAY_MS(500);
		FEB_PIN_RST(PN_PC_REL);
		{
			uint8_t control = CHARGE_CTRL_START;
			(void)osMessageQueuePut(Charging_ControlHandle, &control, 0, 0);
		}
		updateStateProtected(next_state);
		break;
	case FEB_SM_ST_BALANCE:
		updateStateProtected(next_state);
		break;
	case FEB_SM_ST_DEFAULT:
		// If Shutdown Circuit (air-) is open
		if (FEB_PIN_RD(PN_SHS_IN)==FEB_RELAY_STATE_OPEN ) {
			ChargingPrechargeTransition(FEB_SM_ST_FREE);
		}

		//Keep air plus open for redundancy
		FEB_PIN_RST(PN_PC_AIR);

		//Hold precharge relay closed for redundancy
		FEB_PIN_SET(PN_PC_REL);

		if (FEB_ADBMS_Precharge_Complete()) {
			ChargingPrechargeTransition(FEB_SM_ST_CHARGING);
		}
		break;


	default:
		return;
	}
}

static void ChargingTransition(FEB_SM_ST_t next_state){
	switch(next_state){

	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
	case FEB_SM_ST_FAULT_CHARGING:
		// Stop Charging
		{
			uint8_t control = CHARGE_CTRL_STOP;
    		(void)osMessageQueuePut(Charging_ControlHandle, &control, 0, 0);
		}
		fault(FEB_SM_ST_FAULT_CHARGING);
		break;

	case FEB_SM_ST_LV:
	case FEB_SM_ST_FREE:
		// Open air+ and precharge relays - stop charging
		FEB_PIN_RST(PN_PC_AIR);
		FEB_PIN_RST(PN_PC_REL);
		{
			uint8_t control = CHARGE_CTRL_STOP;
    		(void)osMessageQueuePut(Charging_ControlHandle, &control, 0, 0);
		}
		updateStateProtected(FEB_SM_ST_FREE);
		break;

	case FEB_SM_ST_DEFAULT:
		bool charge_status = FEB_CAN_Charging_Status();
		//If accumulator charing status is out of bounds or shutdown circuit(air-) open
		if (!charge_status || FEB_PIN_RD(PN_AIRM_SENSE)==FEB_RELAY_STATE_OPEN ) {
			ChargingTransition(FEB_SM_ST_FREE);
		}

	default:
		break;
	}

}

static void BalanceTransition(FEB_SM_ST_t next_state){
	switch(next_state){

	case FEB_SM_ST_FAULT_BMS:
	case FEB_SM_ST_FAULT_IMD:
		//Stop Balancing
		{
			uint8_t control = BAL_CTRL_STOP;
    		(void)osMessageQueuePut(Balance_ControlHandle, &control, 0, 0);
		}
		fault(FEB_SM_ST_FAULT_CHARGING);
		break;

	case FEB_SM_ST_LV:
	case FEB_SM_ST_FREE:
		// Open air+ and precharge relays for redundancy
		FEB_PIN_RST(PN_PC_AIR);
		FEB_PIN_RST(PN_PC_REL);
		{
			uint8_t control = BAL_CTRL_STOP;
    		(void)osMessageQueuePut(Balance_ControlHandle, &control, 0, 0);
		}
		updateStateProtected(FEB_SM_ST_FREE);
		break;

	case FEB_SM_ST_DEFAULT:
		bool balance_status = FEB_Cell_Balancing_Status();
		//If accumulator charing status is out of bounds or shutdown circuit(air-) open
		if (!balance_status || FEB_PIN_RD(PN_AIRM_SENSE)==FEB_RELAY_STATE_OPEN ) {
			ChargingTransition(FEB_SM_ST_FREE);
		}
		break;

	default:
		break;

	}
}

static void BMSFaultTransition(FEB_SM_ST_t next_state){

	switch(next_state){
	case FEB_SM_ST_DEFAULT:
		// Turn on Fault Indicator
		FEB_PIN_SET(PN_BMS_IND);
		fault(FEB_SM_ST_FAULT_BMS);
		break;

	default:
		break;
	}

}

static void BSPDFaultTransition(FEB_SM_ST_t next_state){
	if (next_state==FEB_SM_ST_DEFAULT)return;
	fault(SM_Current_State);
}
static void IMDFaultTransition(FEB_SM_ST_t next_state){
	if (next_state==FEB_SM_ST_DEFAULT)return;
	fault(SM_Current_State);
}
static void ChargingFaultTransition(FEB_SM_ST_t next_state){
	if (next_state==FEB_SM_ST_DEFAULT)return;
	FEB_PIN_SET(PN_BMS_IND);
	fault(SM_Current_State);
}



