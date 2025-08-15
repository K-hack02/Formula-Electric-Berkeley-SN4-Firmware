// ********************************** Includes & Externs ************************

#include "FEB_CAN.h"
#include "FEB_CAN_PCU.h"
#include "FEB_CAN_BMS.h"
#include "FEB_CAN_RMS.h"
#include "FEB_CAN_LVPDB.h"

extern CAN_HandleTypeDef hcan1;

// ********************************** CAN Configuration *************************

CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
static CAN_RxHeaderTypeDef FEB_CAN_Rx_Header;

uint8_t FEB_CAN_Tx_Data[8];
uint8_t FEB_CAN_Rx_Data[8];

uint32_t FEB_CAN_Tx_Mailbox;

// ********************************** Functions *********************************

void FEB_CAN_Init(void) {
	FEB_CAN_Filter_Config();
	if (HAL_CAN_Start(&hcan1) != HAL_OK) {
        // Code Error - Shutdown
	}
	HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void FEB_CAN_Filter_Config(void) {
	uint8_t filter_bank = 0;
    filter_bank = FEB_CAN_PCU_Filter_Config(&hcan1, CAN_RX_FIFO0, filter_bank);
    filter_bank = FEB_CAN_BMS_Filter_Config(&hcan1, CAN_RX_FIFO0, filter_bank);
	filter_bank = FEB_CAN_LVPDB_Filter_Config(&hcan1, CAN_RX_FIFO0, filter_bank);
	filter_bank = FEB_CAN_RMS_Filter_Config(&hcan1, CAN_RX_FIFO0, filter_bank);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
	if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &FEB_CAN_Rx_Header, FEB_CAN_Rx_Data) != HAL_OK) {
		return;
	}

	FEB_CAN_PCU_Str_Msg(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);
	FEB_CAN_BMS_Store_Msg(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);
	FEB_CAN_LVPDB_Store_Msg(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);
	FEB_CAN_RMS_Store_Msg(&FEB_CAN_Rx_Header, FEB_CAN_Rx_Data);
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
