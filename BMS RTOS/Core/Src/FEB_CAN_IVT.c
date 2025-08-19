// ********************************** Includes & Externs *************************

#include "FEB_CAN_IVT.h"
#include "FEB_SM.h"
#include "FEB_Const.h"
#include "FEB_ADBMS6830B.h"

#include <stdio.h>
#include <string.h>
#include <stdatomic.h>

extern UART_HandleTypeDef huart2;

// ********************************** IVT Frame IDs (Std 11-bit) *****************

#define FEB_CAN_ID_IVT_COMMAND          0x411
#define FEB_CAN_ID_IVT_DEBUG            0x510
#define FEB_CAN_ID_IVT_RESPONSE         0x511
#define FEB_CAN_ID_IVT_CURRENT          0x521
#define FEB_CAN_ID_IVT_VOLTAGE_1        0x522
#define FEB_CAN_ID_IVT_VOLTAGE_2        0x523
#define FEB_CAN_ID_IVT_VOLTAGE_3        0x524
#define FEB_CAN_ID_IVT_TEMPERATURE      0x525
#define FEB_CAN_ID_IVT_POWER            0x526
#define FEB_CAN_ID_IVT_CURRENT_COUNTER  0x527
#define FEB_CAN_ID_IVT_ENERGY_COUNTER   0x528


// ********************************** Structs ************************************

typedef struct {
    atomic_uint seq;
    float current_mA;
    float voltage_1_mV;
    float voltage_2_mV;
    float voltage_3_mV;
} FEB_CAN_IVT_Message_t;

typedef struct {
    atomic_bool current;
    atomic_bool voltage_1;
    atomic_bool voltage_2;
    atomic_bool voltage_3;
} IVT_CAN_flag_t;

// ********************************** Variables **********************************

static IVT_CAN_flag_t IVT_CAN_flag;
FEB_CAN_IVT_Message_t FEB_CAN_IVT_Message = {ATOMIC_VAR_INIT(false), ATOMIC_VAR_INIT(false), ATOMIC_VAR_INIT(false), ATOMIC_VAR_INIT(false)};

// ********************************** Static Functions ***************************

static inline void ivt_write_begin(void) {
    atomic_fetch_add_explicit(&FEB_CAN_IVT_Message.seq, 1u, memory_order_relaxed);  // -> odd
}
static inline void ivt_write_end(void) {
    atomic_fetch_add_explicit(&FEB_CAN_IVT_Message.seq, 1u, memory_order_release);  // -> even
}

static inline float ivt_read_field_consistent(float const *field) {
    unsigned s0, s1;
    float v;
    do {
        s0 = atomic_load_explicit(&FEB_CAN_IVT_Message.seq, memory_order_acquire);
        if (s0 & 1u) continue;
        v  = *field;
        s1 = atomic_load_explicit(&FEB_CAN_IVT_Message.seq, memory_order_acquire);
    } while (s0 != s1 || (s1 & 1u));
    return v;
}

// ********************************** Functions **********************************

uint8_t FEB_CAN_IVT_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {FEB_CAN_ID_IVT_CURRENT, FEB_CAN_ID_IVT_VOLTAGE_1, FEB_CAN_ID_IVT_VOLTAGE_2, FEB_CAN_ID_IVT_VOLTAGE_3};

	for (uint8_t i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
		CAN_FilterTypeDef filter_config;

	    // Standard CAN - 2.0A - 11 bit
	    filter_config.FilterActivation = CAN_FILTER_ENABLE;
		filter_config.FilterBank = filter_bank;
		filter_config.FilterFIFOAssignment = FIFO_assignment;
		filter_config.FilterIdHigh = ids[i] << 5;
		filter_config.FilterIdLow = 0;
		filter_config.FilterMaskIdHigh = 0xFFE0;
		filter_config.FilterMaskIdLow = 0;
		filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
		filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
		filter_config.SlaveStartFilterBank = 27;
	    filter_bank++;

		if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
			//Code Error - shutdown
		}
	}

	return filter_bank;
}

void FEB_CAN_IVT_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]) {
	switch(rx_header->StdId) {
	    case FEB_CAN_ID_IVT_CURRENT:
			int32_t raw_current = (int32_t)((rx_data[2] << 24) | (rx_data[3] << 16) | (rx_data[4] << 8) | rx_data[5]);
            float val_mA = ((float)raw_current) * (-0.001f) * 1000.0f; // correct IVT for reversed direction
            ivt_write_begin();
            FEB_CAN_IVT_Message.current_mA = val_mA;
            ivt_write_end();
            atomic_store_explicit(&IVT_CAN_flag.current, true, memory_order_relaxed);
            break;

	    case FEB_CAN_ID_IVT_VOLTAGE_1:
			uint32_t raw_voltage_1 = ((uint32_t)rx_data[2] << 24) | ((uint32_t)rx_data[3] << 16) | ((uint32_t)rx_data[4] << 8) | (uint32_t)rx_data[5];
            ivt_write_begin();
            FEB_CAN_IVT_Message.voltage_1_mV = (float)raw_voltage_1;
            ivt_write_end();
            atomic_store_explicit(&IVT_CAN_flag.voltage_1, true, memory_order_relaxed);
            break;

	    case FEB_CAN_ID_IVT_VOLTAGE_2:
			uint32_t raw_voltage_2 = ((uint32_t)rx_data[2] << 24) | ((uint32_t)rx_data[3] << 16) | ((uint32_t)rx_data[4] << 8) | (uint32_t)rx_data[5];
            ivt_write_begin();
            FEB_CAN_IVT_Message.voltage_2_mV = (float)raw_voltage_2;
            ivt_write_end();
            atomic_store_explicit(&IVT_CAN_flag.voltage_2, true, memory_order_relaxed);
            break;

	    case FEB_CAN_ID_IVT_VOLTAGE_3:
			uint32_t raw_voltage_3 = ((uint32_t)rx_data[2] << 24) | ((uint32_t)rx_data[3] << 16) | ((uint32_t)rx_data[4] << 8) | (uint32_t)rx_data[5];
            ivt_write_begin();
            FEB_CAN_IVT_Message.voltage_3_mV = (float)raw_voltage_3;
            ivt_write_end();
            atomic_store_explicit(&IVT_CAN_flag.voltage_3, true, memory_order_relaxed);
            break;
		
		default: 
			break;
	}
}

void ivt_read_consistent(float *cur, float *v1, float *v2, float *v3) {
    unsigned s0, s1;
    do {
        s0 = atomic_load_explicit(&FEB_CAN_IVT_Message.seq, memory_order_acquire);
        if (s0 & 1u) continue;
        float c  = FEB_CAN_IVT_Message.current_mA;
        float x1 = FEB_CAN_IVT_Message.voltage_1_mV;
        float x2 = FEB_CAN_IVT_Message.voltage_2_mV;
        float x3 = FEB_CAN_IVT_Message.voltage_3_mV;
        s1 = atomic_load_explicit(&FEB_CAN_IVT_Message.seq, memory_order_acquire);
        if (s0 == s1 && !(s1 & 1u)) { *cur=c; *v1=x1; *v2=x2; *v3=x3; return; }
    } while (1);
}

// Must apply threading safety (Didn't because not being used)
void FEB_CAN_IVT_Process(void) {
	if (IVT_CAN_flag.current) {
		IVT_CAN_flag.current = false;
		// ...
	}
	if (IVT_CAN_flag.voltage_1) {
		IVT_CAN_flag.voltage_1 = false;
		// ...
	}
	if (IVT_CAN_flag.voltage_2) {
		IVT_CAN_flag.voltage_2 = false;
		// ...
	}
	if (IVT_CAN_flag.voltage_3) {
		IVT_CAN_flag.voltage_3 = false;
		// ...
	}
}

int32_t FEB_IVT_V1_Voltage(void) {
    return (int32_t)ivt_read_field_consistent(&FEB_CAN_IVT_Message.voltage_1_mV);
}

int32_t FEB_IVT_V1_Current(void) {
    return (int32_t)ivt_read_field_consistant(&FEB_CAN_IVT_Message.current_mA);
}
