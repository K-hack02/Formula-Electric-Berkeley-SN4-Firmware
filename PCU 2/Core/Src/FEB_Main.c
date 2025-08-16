// ********************************** Includes & External **********************************

#include "FEB_Main.h"
#include "FEB_CAN.h"
#include "TPS2482.h"
#include "FEB_CAN_BMS.h"
#include "FEB_CAN_RMS.h"
#include "FEB_CAN_TPS.h"
#include "FEB_CAN_DASH.h"
#include "FEB_CAN_AUTO.h"

extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern bool auto_on;
extern bool torque;

// ********************************** Variables **********************************

static TPS2482_Configuration tps2482_configurations[1];
uint8_t tps2482_i2c_addresses[1];
static uint16_t tps2482_ids[1];

// ********************************** Functions **********************************

static void FEB_Variable_Init(void) {
	tps2482_i2c_addresses[0] = TPS2482_I2C_ADDR(TPS2482_I2C_ADDR_GND, TPS2482_I2C_ADDR_GND);
	tps2482_configurations[0].config = TPS2482_CONFIG_DEFAULT;
	tps2482_configurations[0].mask = TPS2482_MASK_SOL;
	tps2482_configurations[0].cal = TPS2482_CAL_EQ(TPS2482_CURRENT_LSB_EQ((double)(4)),(double)(.002));
}

void FEB_Main_Setup(void){
	HAL_ADCEx_InjectedStart(&hadc1);
	FEB_CAN_Init();
	FEB_CAN_RMS_Setup();
	FEB_CAN_HEARTBEAT_Init();
	FEB_Variable_Init();
	
	bool tps2482_init_res[1];
	TPS2482_Init(&hi2c1, tps2482_i2c_addresses, tps2482_configurations, tps2482_ids, tps2482_init_res, 1);

	if (!tps2482_init_res[0]) {
		// TODO Update CAN Hertbeat
	}
}

void FEB_Main_Loop(void){
	FEB_SM_ST_t bms_state = FEB_CAN_BMS_getState();
	bool ready_to_drive = FEB_Ready_To_Drive();

	if (!auto_on){
		if (ready_to_drive && (bms_state == FEB_SM_ST_DRIVE)) {
			FEB_Update_Normalized_Acc();
			FEB_CAN_RMS_Process();

		} else {
			FEB_Set_Normalized_Acc_0();
			FEB_CAN_RMS_Disable();
		}

		FEB_HECS_update();
		FEB_CAN_RMS_Torque();
	} else {
		if (bms_state == FEB_SM_ST_ENERGIZED) {
			FEB_CAN_RMS_Process();
		}else {
			FEB_Set_Normalized_Acc_0();
			FEB_CAN_RMS_Disable();
		}
		FEB_CAN_RMS_AUTO_Torque(torque);
	}

	FEB_CAN_Brake();
	FEB_CAN_HEARTBEAT_Transmit();
	FEB_CAN_APPS();
	FEB_CAN_TPS_Transmit();

	HAL_Delay(10);
}
