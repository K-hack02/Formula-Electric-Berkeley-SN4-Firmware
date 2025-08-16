// ********************************** Includes & Externs *************************

#include "FEB_CAN_Charger.h"
#include "FEB_SM.h"
#include "FEB_Config.h"
#include "FEB_ADBMS6830B.h"

#include <string.h>
#include <stdio.h>
#include <stddef.h>

extern CAN_HandleTypeDef hcan1;
extern UART_HandleTypeDef huart2;
extern uint8_t FEB_CAN_Tx_Data[8];
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint32_t FEB_CAN_Tx_Mailbox;

// ********************************** Charger CAN IDs ****************************

#define FEB_CAN_ID_CHARGER_BMS 0x1806e5f4
#define FEB_CAN_ID_CHARGER_CCS 0x18FF50E5

// ********************************** Structs ************************************

typedef struct {
	uint16_t max_voltage_dV;		// Deci-volts
	uint16_t max_current_dA;		// Deci-amps
	uint8_t control;				// 0 (start charge), 1 (stop charge)
	bool done_charging;
} BMS_Charger_Message_t;

typedef struct {
	uint16_t op_voltage_dV;			// Operating voltage
	uint16_t op_current_dA;			// Operating current
	uint8_t status;
	bool received;
} CCS_message_t;

// ********************************** Variables **********************************

static BMS_Charger_Message_t BMS_Charger_Message;
static CCS_message_t CCS_message;


static bool charging_on = true;

static bool     trickle_charge_enabled = false;
static bool     trickle_pwm_on         = true;
static uint32_t trickle_last_toggle_ms = 0;

// ********************************** Internals **********************************

static void charger_CAN_transmit(void);

// ********************************** Functions **********************************

void FEB_CAN_Charger_Init(void) {
	BMS_Charger_Message.max_voltage_dV = (uint16_t)(FEB_NBANKS * FEB_NUM_CELL_PER_BANK * (uint16_t)(FEB_Config_Get_Cell_Max_Voltage_mV() * 1e-2));
	BMS_Charger_Message.max_current_dA = CHARGE_CURRENT_dA;
	BMS_Charger_Message.done_charging = false;

	CCS_message.received = false;
}

uint8_t FEB_CAN_Charger_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	CAN_FilterTypeDef CAN_filter_config;

	CAN_filter_config.FilterActivation = CAN_FILTER_ENABLE;
	CAN_filter_config.FilterBank = filter_bank;
	CAN_filter_config.FilterFIFOAssignment = FIFO_assignment;
	CAN_filter_config.FilterIdHigh = (uint32_t)(FEB_CAN_ID_CHARGER_CCS >> 13);
	CAN_filter_config.FilterIdLow = (uint32_t)((FEB_CAN_ID_CHARGER_CCS & 0x1FFF) << 3);
	CAN_filter_config.FilterMaskIdHigh = 0xFFFF;
	CAN_filter_config.FilterMaskIdLow = 0xFFF8;
	CAN_filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	CAN_filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	CAN_filter_config.SlaveStartFilterBank = 27;
	filter_bank++;

	if (HAL_CAN_ConfigFilter(hcan, &CAN_filter_config) != HAL_OK) {
		// Code Error - Shutdown
	}

	return filter_bank++;
}

void FEB_CAN_Charger_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]) {
	switch(rx_header->ExtId) {
	    case FEB_CAN_ID_CHARGER_CCS:
	    	CCS_message.op_voltage_dV = (uint16_t) (rx_data[0] << 8) + rx_data[1];
	    	CCS_message.op_current_dA = (uint16_t) (rx_data[2] << 8) + rx_data[3];
	    	CCS_message.status = rx_data[4];
	    	CCS_message.received = true;
			break;
	}
}

static void charger_CAN_transmit(void) {
	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 8;
	FEB_CAN_Tx_Header.ExtId = FEB_CAN_ID_CHARGER_BMS;
	FEB_CAN_Tx_Header.IDE = CAN_ID_EXT;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer
	FEB_CAN_Tx_Data[0] = BMS_Charger_Message.max_voltage_dV >> 8;
	FEB_CAN_Tx_Data[1] = BMS_Charger_Message.max_voltage_dV & 0xFF;
	FEB_CAN_Tx_Data[2] = BMS_Charger_Message.max_current_dA >> 8;
	FEB_CAN_Tx_Data[3] = BMS_Charger_Message.max_current_dA & 0xFF;
	FEB_CAN_Tx_Data[4] = BMS_Charger_Message.control;
	FEB_CAN_Tx_Data[5] = 0;
	FEB_CAN_Tx_Data[6] = 0;
	FEB_CAN_Tx_Data[7] = 0;

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		//Code Error - shutdown
	}
}

static void charger_set_limits(uint16_t max_voltage_dV, uint16_t max_current_dA, uint8_t control) {
    BMS_Charger_Message.max_voltage_dV = max_voltage_dV;
    BMS_Charger_Message.max_current_dA = max_current_dA;
    BMS_Charger_Message.control        = control;
}


void FEB_CAN_Charger_Process(void) {
	if (FEB_SM_Get_Current_State() != FEB_SM_ST_CHARGING) {
		trickle_charge_enabled = false;
        trickle_pwm_on         = true;
        charging_on            = false;
        return;
	}

	const uint32_t now_ms = HAL_GetTick();
	uint16_t current_pack_voltage_dV = (uint16_t)(FEB_ADBMS_GET_ACC_Total_Voltage() * 10.0);

	if (!trickle_charge_enabled) {
        if (current_pack_voltage_dV >= TRICKLE_CHARGE_START_VOLTAGE_dV) {
            trickle_charge_enabled = true;
            trickle_pwm_on         = true;
            trickle_last_toggle_ms = now_ms;
        }
    } else {
        if (current_pack_voltage_dV <= (TRICKLE_CHARGE_MAX_VOLTAGE_dV - TRICKLE_EXIT_HYSTERESIS_dV)) {
            trickle_charge_enabled = false;
            trickle_pwm_on         = true;
        }
    }

	if (trickle_charge_enabled) {
		const uint32_t elapsed = now_ms - trickle_last_toggle_ms;

		if (trickle_pwm_on) {
            if (elapsed >= TRICKLE_PWM_ON_MS) {
                trickle_pwm_on = false;
                trickle_last_toggle_ms = now_ms;
            }
        } else {
            if (elapsed >= TRICKLE_PWM_OFF_MS) {
                trickle_pwm_on = true;
                trickle_last_toggle_ms = now_ms;
            }
        }

		if (trickle_pwm_on) {
            charger_set_limits(TRICKLE_CHARGE_MAX_VOLTAGE_dV, TRICKLE_CHARGE_CURRENT_dA, 0);
            charging_on = true;
        } else {
            charger_set_limits(TRICKLE_CHARGE_MAX_VOLTAGE_dV, 0, 0);
            charging_on = false;
		}
    } else {
		charger_set_limits(TRICKLE_CHARGE_MAX_VOLTAGE_dV, CHARGE_CURRENT_dA, 0);
        charging_on = true;
    }
	charger_CAN_transmit();
}


void FEB_CAN_Charger_Start_Charge(void) {
	BMS_Charger_Message.control = 0;
}

void FEB_CAN_Charger_Stop_Charge(void) {
	BMS_Charger_Message.control = 1;
	BMS_Charger_Message.done_charging = true;

}

bool FEB_CAN_Charger_Received(void){
	return CCS_message.received;
}

int8_t FEB_CAN_Charging_Status(void) {
	if (BMS_Charger_Message.done_charging == true) {
		return 1;
	}

	if (FEB_ADBMS_GET_ACC_Total_Voltage() >= FEB_CONFIG_PACK_HARD_MAX_VOLTAGE_V ) {
		return -1;
	}

	for ( size_t i = 0; i < FEB_NBANKS; ++i) {
		for ( size_t j = 0; j < FEB_NUM_CELL_PER_BANK; ++j) {
			
			float cell_voltage = FEB_ADBMS_GET_Cell_Voltage(i, j);
			if (cell_voltage * 1000 >= FEB_CONFIG_CELL_SOFT_MAX_VOLTAGE_mV ) {
				if (cell_voltage * 1000 >= FEB_CONFIG_CELL_HARD_MAX_VOLTAGE_mV ) {
					return -1;
				}
				return 1;
			}
			
			float cell_temp = FEB_ADBMS_GET_Cell_Temperature(i, j);
			if (cell_temp * 10  >= FEB_CONFIG_CELL_SOFT_MAX_TEMP_dC ) {
				if (cell_temp * 10 >= FEB_CONFIG_CELL_HARD_MAX_TEMP_dC ) {
					return -1;
				}
				return 1;
			}
		}
	}

	return 0;
}

void FEB_CAN_Charger_Serial(void) {
	static char str[128];
	
	sprintf(str, "charger1 %d %d %d\n",BMS_Charger_Message.max_voltage_dV, BMS_Charger_Message.max_current_dA, BMS_Charger_Message.control);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
	
	sprintf(str, "charger2 %d %d %d\n", CCS_message.op_voltage_dV, CCS_message.op_current_dA, CCS_message.status);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
}

void FEB_CAN_Charger_UART_Transmit(void) {
	static char str[128];
	
	sprintf(str, "Charger: Max_V: %d Max_A: %d Control: %d OP_V: %d OP_A: %d Status: %d Recieved: %d\n",
		BMS_Charger_Message.max_voltage_dV, BMS_Charger_Message.max_current_dA, BMS_Charger_Message.control,
		CCS_message.op_voltage_dV, CCS_message.op_current_dA, CCS_message.status, CCS_message.received);

	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
}

