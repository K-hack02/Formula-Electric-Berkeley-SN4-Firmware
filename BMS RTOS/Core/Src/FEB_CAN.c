// ********************************** Includes & Externs ************************

#include "FEB_CAN.h"
#include "FEB_HW.h"
#include "FEB_SM.h"
#include "FEB_Const.h"
#include "FEB_Config.h"
#include "FEB_CAN_IDs.h"
#include "FEB_CAN_IVT.h"
#include "FEB_CAN_DASH.h"
#include "FEB_ADBMS6830B.h"
#include "FEB_CAN_Charger.h"
#include "FEB_CAN_Heartbeat.h"

#include <math.h>
#include <string.h>
#include <stddef.h>

extern CAN_HandleTypeDef  hcan1;
extern uint8_t            ERROR_TYPE;

// ********************************** CAN Configuration *************************

CAN_TxHeaderTypeDef   FEB_CAN_Tx_Header;
static CAN_RxHeaderTypeDef FEB_CAN_Rx_Header;
static FEB_DEV_INDEX  ping_alive;

uint8_t   FEB_CAN_Tx_Data[8];
uint8_t   FEB_CAN_Rx_Data[8];
uint32_t  FEB_CAN_Tx_Mailbox;

// ********************************** Functions *********************************

void FEB_CAN_Init(void) {
	FEB_CAN_Filter_Config();
	if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        // Code Error - Shutdown
	}

	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
	ping_alive = (FEB_DEV_INDEX)0;
}

void FEB_CAN_Filter_Config(void) {
	uint8_t filter_bank = 0;
	filter_bank = FEB_CAN_IVT_Filter_Config(&hcan1, CAN_RX_FIFO0, filter_bank);
	filter_bank = FEB_CAN_DASH_Filter_Config(&hcan1,CAN_RX_FIFO0, filter_bank);
	filter_bank = FEB_CAN_Charger_Filter_Config(&hcan1,CAN_RX_FIFO0, filter_bank);
	filter_bank = FEB_CAN_Heartbeat_Filter_Config(&hcan1,CAN_RX_FIFO0, filter_bank);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {

	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &FEB_CAN_Rx_Header, FEB_CAN_Rx_Data) != HAL_OK) {
		return;
	}

	FEB_CAN_IVT_Store_Msg(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);
	FEB_CAN_DASH_Store_Msg(&FEB_CAN_Rx_Header,FEB_CAN_Rx_Data);
	FEB_CAN_Charger_Store_Msg(&FEB_CAN_Rx_Header,FEB_CAN_Rx_Data);
	FEB_CAN_Heartbeat_Store_Msg(&FEB_CAN_Rx_Header,FEB_CAN_Rx_Data);
}

void FEB_SM_CAN_Transmit(void) {
    uint8_t relay_state = ((FEB_PIN_RD(PN_PC_REL) & 1U) << 2) | ((FEB_PIN_RD(PN_AIRP_SENSE) & 1U) << 1) |  (FEB_PIN_RD(PN_SHS_IN) & 1U);
    uint8_t gpio_sense  = relay_state;

    uint8_t data[2];
    data[0]  = (uint8_t)FEB_SM_Get_Current_State();
    data[0] |= (uint8_t)((ping_alive & 0x0F) << 5);
    data[1]  = (uint8_t)(((gpio_sense & 0x3F) << 3) | (relay_state & 0x03));

    (void)FEB_CAN_Send_Std(FEB_CAN_BMS_STATE_FRAME_ID, data, (uint8_t)sizeof(data));

	FEB_CAN_Heartbeat_Reset_Last_Received(ping_alive);

    if (FEB_CAN_Heartbeat_Get_Initialized(ping_alive) > 0) {
        FEB_CAN_Heartbeat_Decrement_Initialized(ping_alive);
    }

    ping_alive = (FEB_DEV_INDEX)((ping_alive + 1) % FEB_NUM_CAN_DEV);

    for (int i = 0; i < (int)FEB_NUM_CAN_DEV; i++) {
        FEB_CAN_Heartbeat_Increment_LaOn((FEB_DEV_INDEX)i);
    }

    FEB_DEV_INDEX prev_device_index = (ping_alive == 0) ? (int)FEB_NUM_CAN_DEV - 1 : (int)ping_alive - 1;
    
    if (FEB_CAN_Heartbeat_Get_Last_Received(prev_device_index) == 0) {
        FEB_CAN_Heartbeat_Increment_FAck(prev_device_index);
    }
}

void FEB_CELL_CAN_Transmit(void)
{
    uint8_t  cell_flags    = 0;
    uint8_t  cell_num      = 0;
    uint8_t  bank_num      = 0;
    uint16_t cell_volt_mV  = 0;
    int16_t  cell_temp_C   = 0;

    uint8_t data[6];
    data[0] = (uint8_t)(cell_flags & 0x0F);
    data[1] = (uint8_t)(((bank_num & 0x0F) << 4) | (cell_num & 0x0F));
    data[2] = (uint8_t)(cell_volt_mV & 0xFF);
    data[3] = (uint8_t)(cell_volt_mV >> 8);
    data[4] = (uint8_t)(cell_temp_C & 0xFF);
    data[5] = (uint8_t)((uint16_t)cell_temp_C >> 8);

    (void)FEB_CAN_Send_Std(FEB_CAN_BMS_CELL_DATA_FRAME_ID, data, (uint8_t)sizeof(data));
}

void FEB_ACC_VOLT_CAN_Transmit(void) {
    float pack_volt_V     = FEB_ADBMS_GET_ACC_Total_Voltage();
    float min_cell_volt_V = FEB_ADBMS_GET_ACC_MIN_Voltage();
    float max_cell_volt_V = FEB_ADBMS_GET_Cell_Voltage_V(0, 0);

    // Pack: pack voltage in dV (x100), cell min/max in mV (x1000)
    uint16_t pack_dV = (uint16_t)(pack_volt_V * 100.0f);
    uint16_t min_mV  = (uint16_t)(min_cell_volt_V * 1000.0f);
    uint16_t max_mV  = (uint16_t)(max_cell_volt_V * 1000.0f);

    uint8_t data[6];
    data[0] = (uint8_t)(pack_dV & 0xFF);
    data[1] = (uint8_t)(pack_dV >> 8);
    data[2] = (uint8_t)(min_mV & 0xFF);
    data[3] = (uint8_t)(min_mV >> 8);
    data[4] = (uint8_t)(max_mV & 0xFF);
    data[5] = (uint8_t)(max_mV >> 8);

    (void)FEB_CAN_Send_Std(FEB_CAN_BMS_ACCUMULATOR_VOLTAGE_FRAME_ID, data, (uint8_t)sizeof(data));
}

void FEB_ACC_TEMP_CAN_Transmit(void) {
    double pack_temp_C     = FEB_ADBMS_GET_ACC_AVG_Temp();
    float  min_cell_temp_C = FEB_ADBMS_GET_ACC_MIN_Temp();
    float  max_cell_temp_C = FEB_ADBMS_GET_ACC_MAX_Temp();

    int16_t pack_cC = (int16_t)(pack_temp_C * 100.0);
    int16_t min_cC  = (int16_t)(min_cell_temp_C * 100.0f);
    int16_t max_cC  = (int16_t)(max_cell_temp_C * 100.0f);

    uint8_t data[6];
    data[0] = (uint8_t)(pack_cC & 0xFF);
    data[1] = (uint8_t)(pack_cC >> 8);
    data[2] = (uint8_t)(min_cC & 0xFF);
    data[3] = (uint8_t)(min_cC >> 8);
    data[4] = (uint8_t)(max_cC & 0xFF);
    data[5] = (uint8_t)(max_cC >> 8);

    (void)FEB_CAN_Send_Std(FEB_CAN_BMS_ACCUMULATOR_TEMPERATURE_FRAME_ID, data, (uint8_t)sizeof(data));
}

void FEB_ACC_FAULTS_CAN_Transmit(void) {
    FEB_Relay_State bms_fault_sense = FEB_PIN_RD(PN_INDICATOR); // closed = fault
    FEB_Relay_State imd_fault_sense = FEB_PIN_RD(PN_SHS_IMD);   // open   = fault

    uint8_t fault = 0x00;
    if (bms_fault_sense == FEB_RELAY_STATE_CLOSE) { 
		fault |= 0x01; 
	}
    if (imd_fault_sense == FEB_RELAY_STATE_OPEN)  { 
		fault |= 0x02; 
	}

    fault |= FEB_ADBMS_Get_Error_Type();

    uint8_t data[1] = { fault };
    (void)FEB_CAN_Send_Std(FEB_CAN_ACCUMULATOR_FAULTS_FRAME_ID, data, (uint8_t)sizeof(data));
}

bool FEB_CAN_Send_Std(uint16_t std_id, const uint8_t *data, uint8_t dlc) {
  CAN_TxHeaderTypeDef hdr;

  hdr.StdId = std_id;
  hdr.ExtId = 0;
  hdr.IDE   = CAN_ID_STD;
  hdr.RTR   = CAN_RTR_DATA;
  hdr.DLC   = dlc;
  hdr.TransmitGlobalTime = DISABLE;

  while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {
    // Code Error - Shutdown
  }

  if (HAL_CAN_AddTxMessage(&hcan1, &hdr, (uint8_t*)data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
    return false;
  }

  return true;
}

// ********************************** Template Code *****************************

uint8_t FEB_CAN_Filter(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
    // For multiple filters, create array of filter IDs and loop over IDs.

	CAN_FilterTypeDef filter_config;

    // Standard CAN - 2.0A - 11 bit
    filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = filter_bank;
	filter_config.FilterFIFOAssignment = FIFO_assignment;
	filter_config.FilterIdHigh = /* ID */ 0 << 5;
	filter_config.FilterIdLow = 0;
	filter_config.FilterMaskIdHigh = 0xFFE0;
	filter_config.FilterMaskIdLow = 0;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 27;
    filter_bank++;

    // Extended CAN - 2.0B - 29 bit
	filter_config.FilterActivation = CAN_FILTER_ENABLE;
	filter_config.FilterBank = filter_bank;
	filter_config.FilterFIFOAssignment = FIFO_assignment;
	filter_config.FilterIdHigh = /* ID */ 0 >> 13;
	filter_config.FilterIdLow = (/* ID */ 0 & 0x1FFF) << 3;
	filter_config.FilterMaskIdHigh = 0xFFFF;
	filter_config.FilterMaskIdLow = 0xFFF8;
	filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
	filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
	filter_config.SlaveStartFilterBank = 27;
    filter_bank++;

	if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
        // Code Error - Shutdown
	}

	return filter_bank;
}

void FEB_CAN_Transmit(CAN_HandleTypeDef* hcan) {
	// Initialize Transmission Header
    // Write Code Here

	// Configure FEB_CAN_Tx_Data
    // Write Code Here

	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {}

	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(hcan, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		// Code Error - Shutdown
	}
}
