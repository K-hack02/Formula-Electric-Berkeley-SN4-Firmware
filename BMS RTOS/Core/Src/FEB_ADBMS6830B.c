// ********************************** Includes & Externs *************************

#include "FEB_ADBMS6830B.h"
#include "FEB_SM.h"
#include "FEB_HW.h"
#include "FEB_CAN_IVT.h"
#include "FEB_CMDCODES.h"
#include "FEB_Cell_Temp_LUT.h"
#include "FEB_AD68xx_Interface.h"
#include "FEB_ADBMS6830B_Driver.h"

#include <float.h>
#include <stdbool.h>
#include <stdatomic.h>
#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

extern osThreadId_t State_MachineHandle;

// ********************************** Variables **********************************

cell_asic IC_Config[FEB_NUM_IC];

static accumulator_t s_acc_buf[2];
static _Atomic(accumulator_t*) s_acc_read  = &s_acc_buf[0];
static accumulator_t*         s_acc_write = &s_acc_buf[1];

int balancing_cycle=0;
uint16_t balancing_mask=0xAAAA;

static bool balancing_active = false;

uint8_t ERROR_TYPE=0; //HEXDIGIT 1 voltage faults; HEXDIGIT 2 temp faults; HEXDIGIT 3 relay faults

// ********************************** Config Bits ********************************

static bool refon = 0;
static bool cth_bits[3] = {1,1,1};
static bool gpio_bits[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 1};
static bool dcto_bits[6] = {1,1,1,1,1,1};
static uint16_t uv = 0x0010;
static uint16_t ov = 0x3FF0;

static float FEB_MIN_SLIPPAGE_V=0.03;

// ********************************** Helper Functions ***************************

static inline float convert_voltage(int16_t raw_code) {
	return raw_code * 0.000150f + 1.5f;
}

// ********************************** Static Functions ***************************

static const accumulator_t* ACC_Read(void) {
    return atomic_load_explicit(&s_acc_read, memory_order_acquire);
}

// ********************************** Voltage ************************************

static void start_adc_cell_voltage_measurements() {
	ADBMS6830B_adcv(1, 0, 1, 0, OWVR);
	RTOS_DELAY_MS(1);
	//ADBMS6830B_pollAdc();
}

static void read_cell_voltages() {
	ADBMS6830B_rdcv(FEB_NUM_IC, IC_Config);
	ADBMS6830B_rdsv(FEB_NUM_IC, IC_Config);
}

static void store_cell_voltages(accumulator_t* FEB_ACC) {
	FEB_ACC->total_voltage_V = 0;
	
	float min_cell_V = FLT_MAX;
    float max_cell_V = -FLT_MAX;

	for (uint8_t bank = 0; bank < FEB_NBANKS; bank ++) {
		for(uint8_t ic =0; ic<FEB_NUM_ICPBANK;ic++){
			for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell ++) {
				float CVoltage = convert_voltage(IC_Config[ic+bank*FEB_NUM_ICPBANK].cells.c_codes[cell]);
				float SVoltage = convert_voltage(IC_Config[ic+bank*FEB_NUM_ICPBANK].cells.s_codes[cell]);

				FEB_ACC->banks[bank].cells[cell+ic*FEB_NUM_CELLS_PER_IC].voltage_V = CVoltage;
				FEB_ACC->banks[bank].cells[cell+ic*FEB_NUM_CELLS_PER_IC].voltage_S = SVoltage;
				FEB_ACC->total_voltage_V+=CVoltage;

				if (CVoltage >= 0.0f) {
                    if (CVoltage < min_cell_V) min_cell_V = CVoltage;
                    if (CVoltage > max_cell_V) max_cell_V = CVoltage;
                }
			}
		}
	}
	FEB_ACC->pack_min_voltage_V = min_cell_V;
	FEB_ACC->pack_max_voltage_V = max_cell_V;
}

static void validate_voltages(accumulator_t* FEB_ACC) {
	uint16_t vMax = FEB_Config_Get_Cell_Max_Voltage_mV();
	uint16_t vMin = FEB_Config_Get_Cell_Min_Voltage_mV();

	for (uint8_t bank = 0; bank < FEB_NBANKS; bank ++) {
		FEB_ACC->banks[bank].badReadV=0;
		for (uint8_t cell = 0; cell < FEB_NUM_CELL_PER_BANK; cell ++) {
			float voltageC = FEB_ACC->banks[bank].cells[cell].voltage_V*1000;
			float voltageS = FEB_ACC->banks[bank].cells[cell].voltage_S*1000;
			if (voltageC > vMax|| voltageC < vMin) {
				if(voltageC==-1000){
					FEB_ACC->banks[bank].badReadV+=1;
				}
				if(voltageS > vMax || voltageS < vMin){
					FEB_ACC->banks[bank].cells[cell].violations+=1;
					if(FEB_ACC->banks[bank].cells[cell].violations == FEB_VOLTAGE_ERROR_THRESH) {
						FEB_ADBMS_Update_Error_Type(0x30);
						xTaskNotify(State_MachineHandle, (uint32_t)FEB_SM_ST_FAULT_BMS, eSetValueWithOverwrite);
					}
				} else {
					FEB_ACC->banks[bank].cells[cell].violations = 0;
				}
			} else {
				FEB_ACC->banks[bank].cells[cell].violations=0;
			}
		}
	}
}

// ********************************** Temperature ********************************

static void configure_gpio_bits(uint8_t channel) {
	gpio_bits[0] = 0b1; /* ADC Channel */
	gpio_bits[1] = 0b1; /* ADC Channel */
	gpio_bits[2] = ((channel >> 0) & 0b1 ); /* MUX Sel 1 */
	gpio_bits[3] = ((channel >> 1) & 0b1 ); /* MUX Sel 1 */
	gpio_bits[4] = ((channel >> 2) & 0b1 ); /* MUX Sel 1 */
	gpio_bits[5] = 0b1; /* ADC Channel */
	gpio_bits[6] = 0b1; /* ADC Channel */
	for (uint8_t icn = 0; icn < FEB_NUM_IC; icn++) {
		ADBMS6830B_set_cfgr(icn, IC_Config, refon, cth_bits, gpio_bits, 0, dcto_bits, uv, ov);
	}
	ADBMS6830B_wrcfga(FEB_NUM_IC, IC_Config);
}

static void start_aux_voltage_measurements() {
	ADBMS6830B_adax(AUX_OW_OFF, PUP_DOWN, 1);
	RTOS_DELAY_MS(2);
	//ADBMS6830B_pollAdc();
	ADBMS6830B_adax(AUX_OW_OFF, PUP_DOWN, 2);
	RTOS_DELAY_MS(2);
}

static void read_aux_voltages() {
	ADBMS6830B_rdaux(FEB_NUM_IC, IC_Config);
}

static void store_cell_temps(uint8_t channel, accumulator_t* FEB_ACC) {
    float total_temp_C = 0.0f;
    uint16_t temp_count = 0;

	float min_temp_C = FLT_MAX;
    float max_temp_C = -FLT_MAX;

	for (uint8_t bank = 0; bank < FEB_NBANKS; bank++) {
		for (uint8_t icn = 0; icn < FEB_NUM_ICPBANK; icn++) {
			uint16_t mux1 = IC_Config[FEB_NUM_ICPBANK*bank+icn].aux.a_codes[0];
			uint16_t mux2 = IC_Config[FEB_NUM_ICPBANK*bank+icn].aux.a_codes[1];
			float V1=(convert_voltage(mux1)*1000);
			float V2=(convert_voltage(mux2)*1000);
			
			float T1 = FEB_Cell_Temp_LUT_Get_Temp_100mC((int)V1) * 0.1f;
            float T2 = FEB_Cell_Temp_LUT_Get_Temp_100mC((int)V2) * 0.1f;

			FEB_ACC->banks[bank].temp_sensor_readings_V[icn*FEB_NUM_TEMP_SENSE_PER_IC+channel] = T1;
			FEB_ACC->banks[bank].temp_sensor_readings_V[icn*FEB_NUM_TEMP_SENSE_PER_IC+channel+5] = T2;

			if (T1 >= 0) {
                if (T1 < min_temp_C) min_temp_C = T1;
                if (T1 > max_temp_C) max_temp_C = T1;
                total_temp_C += T1;
                temp_count++;
            }

		}
	}

	if (temp_count > 0) {
        FEB_ACC->pack_min_temp = min_temp_C;
        FEB_ACC->pack_max_temp = max_temp_C;
        FEB_ACC->average_pack_temp = total_temp_C / (double)temp_count;
    }
}

static void validate_temps(accumulator_t* FEB_ACC) {
	uint16_t tMax = FEB_Config_Get_Cell_Max_Temperature_dC();
	uint16_t tMin = FEB_Config_Get_Cell_Min_Temperature_dC();
	int totalReads=0;

	for (uint8_t bank = 0; bank < FEB_NBANKS; bank ++) {
		FEB_ACC->banks[bank].tempRead = 0;
		for (uint8_t cell = 0; cell < FEB_NUM_CELL_PER_BANK; cell ++) {
			float temp = FEB_ACC->banks[bank].temp_sensor_readings_V[cell]*10;
			if(temp!=-12.0f)FEB_ACC->banks[bank].tempRead+=1;
			else continue;
			if (temp > tMax || temp < (float) tMin) {
				FEB_ACC->banks[bank].temp_violations[cell]++;
				if(FEB_ACC->banks[bank].temp_violations[cell] == FEB_TEMP_ERROR_THRESH){
					FEB_ADBMS_Update_Error_Type(0x10);
					xTaskNotify(State_MachineHandle, (uint32_t)FEB_SM_ST_FAULT_BMS, eSetValueWithOverwrite);
				}
			} else {
				FEB_ACC->banks[bank].temp_violations[cell] = 0;
			}
		}
		totalReads+=FEB_ACC->banks[bank].tempRead;
	}

	if(totalReads/(float)(FEB_NUM_CELL_PER_BANK*FEB_NBANKS) < 0.2){
		FEB_ADBMS_Update_Error_Type(0x20);
		xTaskNotify(State_MachineHandle, (uint32_t)FEB_SM_ST_FAULT_BMS, eSetValueWithOverwrite);

	}
}

// ********************************** Balancing **********************************

static void determineMinV(accumulator_t* FEB_ACC){
	transmitCMD(ADCV|AD_CONT|AD_RD);
	RTOS_DELAY_MS(1);
	read_cell_voltages();
	store_cell_voltages(FEB_ACC);
	validate_voltages(FEB_ACC);
}

// ********************************** Functions **********************************

void FEB_ADBMS_Init() {

	accumulator_t* a1 = &s_acc_buf[0];
	accumulator_t* a2 = &s_acc_buf[1];

	for (uint8_t bank = 0; bank < FEB_NBANKS; bank ++) {
		a1->banks[bank].badReadV = 0;
		a1->banks[bank].tempRead = 0;
		a1->banks[bank].total_voltage_V = 0;

		a2->banks[bank].badReadV = 0;
		a2->banks[bank].tempRead = 0;
		a2->banks[bank].total_voltage_V = 0;

		for (uint8_t ic = 0; ic < FEB_NUM_ICPBANK; ic++) {
			for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell++) {
				a1->banks[bank].cells[cell].voltage_V = 0;
				a1->banks[bank].cells[cell].voltage_S = 0;
				a1->banks[bank].cells[cell].violations = 0;
				a1->banks[bank].cells[cell].discharging = 0;
				a1->banks[bank].temp_violations[cell] = 0;

				a2->banks[bank].cells[cell].voltage_V = 0;
				a2->banks[bank].cells[cell].voltage_S = 0;
				a2->banks[bank].cells[cell].violations = 0;
				a2->banks[bank].cells[cell].discharging = 0;
				a2->banks[bank].temp_violations[cell] = 0;
			}
		}
	}

	atomic_store_explicit(&s_acc_read, &s_acc_buf[0], memory_order_release);
    s_acc_write = &s_acc_buf[1];

	FEB_cs_high();
	ADBMS6830B_init_cfg(FEB_NUM_IC, IC_Config);
	ADBMS6830B_reset_crc_count(FEB_NUM_IC, IC_Config);
	ADBMS6830B_init_reg_limits(FEB_NUM_IC, IC_Config);
	ADBMS6830B_wrALL(FEB_NUM_IC, IC_Config);
}

accumulator_t* ACC_BeginWrite(void) {
    const accumulator_t* cur = ACC_Read();
    *s_acc_write = *cur;         
    return s_acc_write;
}

void ACC_Publish(void) {
    accumulator_t* old = atomic_exchange_explicit(&s_acc_read, s_acc_write, memory_order_release);
    s_acc_write = old;
}

void FEB_ADBMS_Voltage_Process(accumulator_t* FEB_ACC) {
	start_adc_cell_voltage_measurements();
	read_cell_voltages();
	store_cell_voltages(FEB_ACC);
	validate_voltages(FEB_ACC);
}

void FEB_ADBMS_Temperature_Process(accumulator_t* FEB_ACC){
	gpio_bits[9]^=0b1;
	for (uint8_t channel = 0; channel < 5; channel++) {
			configure_gpio_bits(channel);
			start_aux_voltage_measurements();
			read_aux_voltages();
			store_cell_temps(channel, FEB_ACC);
	}
	validate_temps(FEB_ACC);       
}

// ********************************** Voltage ************************************

float FEB_ADBMS_GET_ACC_Total_Voltage() {
	return ACC_Read()->total_voltage_V;
}

float FEB_ADBMS_GET_ACC_MIN_Voltage() {
	return ACC_Read()->pack_min_voltage_V;
}

float FEB_ADBMS_GET_ACC_MAX_Voltage() {
    return ACC_Read()->pack_max_voltage_V;
}

float FEB_ADBMS_GET_Cell_Voltage_V(uint8_t bank, uint16_t cell) {
    const accumulator_t* FEB_ACC = ACC_Read();
    if (bank >= FEB_NBANKS || cell >= FEB_NUM_CELL_PER_BANK) return -1.0f;
    return FEB_ACC->banks[bank].cells[cell].voltage_V;
}

float FEB_ADBMS_GET_Cell_Voltage_S(uint8_t bank, uint16_t cell) {
    const accumulator_t* FEB_ACC = ACC_Read();
    if (bank >= FEB_NBANKS || cell >= FEB_NUM_CELL_PER_BANK) return -1.0f;
    return FEB_ACC->banks[bank].cells[cell].voltage_S;
}

uint8_t FEB_ADBMS_GET_Cell_Discharging(uint8_t bank, uint16_t cell) {
    const accumulator_t* FEB_ACC = ACC_Read();
    if (bank >= FEB_NBANKS || cell >= FEB_NUM_CELL_PER_BANK) return -1.0f;
    return FEB_ACC->banks[bank].cells[cell].discharging;
}

bool FEB_ADBMS_Precharge_Complete(void) {
    float voltage_V = (float)FEB_IVT_V1_Voltage() * 0.001f;
    return (voltage_V >= (0.9f * FEB_ADBMS_GET_ACC_Total_Voltage()));
}

// ********************************** Temperature ********************************

float FEB_ADBMS_GET_ACC_AVG_Temp() {
	return ACC_Read()->average_pack_temp;
}

float FEB_ADBMS_GET_ACC_MIN_Temp() {
	return ACC_Read()->pack_min_temp;
}

float FEB_ADBMS_GET_ACC_MAX_Temp() {
	return ACC_Read()->pack_max_temp;
}

float FEB_ADBMS_GET_Cell_Temperature(uint8_t bank, uint16_t cell) {
    const accumulator_t* FEB_ACC = ACC_Read();
    if (bank >= FEB_NBANKS || cell >= FEB_NUM_CELL_PER_BANK) return -1.0f;
    return FEB_ACC->banks[bank].temp_sensor_readings_V[cell];
}


// ********************************** Balancing **********************************

void FEB_Cell_Balance_Start(){
	FEB_cs_high();
	ADBMS6830B_init_cfg(FEB_NUM_IC, IC_Config);
	ADBMS6830B_wrALL(FEB_NUM_IC, IC_Config);
	balancing_active = true;
}

void FEB_Cell_Balance_Process(){
	if (!balancing_active) {
        return;
    }

	if (FEB_SM_Get_Current_State() != FEB_SM_ST_BALANCE) {
		FEB_Stop_Balance();
        return;
	}

	if(balancing_cycle == 3 ){
		balancing_mask=~balancing_mask;
		balancing_cycle=0;
	}
	balancing_cycle++;

	accumulator_t* FEB_ACC = ACC_BeginWrite();	// writes to discharging variable but doesn't publish (next cycle ADBMS Process will)
	FEB_Stop_Balance();
	determineMinV(FEB_ACC);
	float MIN_CELL_VOLTAGE = FEB_ACC->pack_min_voltage_V;

	for (uint8_t icn = 0; icn < FEB_NUM_IC; icn ++) {
		uint16_t bits=0x0000;
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC; cell ++) {
			float volt = FEB_ACC->banks[icn/FEB_NUM_ICPBANK].cells[cell+FEB_NUM_CELLS_PER_IC*(icn%FEB_NUM_ICPBANK)].voltage_V;
			float diff = volt-MIN_CELL_VOLTAGE;
			if(diff>FEB_MIN_SLIPPAGE_V){
				bits|=(0b1<<cell);
				FEB_ACC->banks[icn/FEB_NUM_ICPBANK].cells[cell+FEB_NUM_CELLS_PER_IC*(icn%FEB_NUM_ICPBANK)].discharging=0b1&((balancing_mask&bits)>>cell);
			} else {
				FEB_ACC->banks[icn/FEB_NUM_ICPBANK].cells[cell+FEB_NUM_CELLS_PER_IC*(icn%FEB_NUM_ICPBANK)].discharging=0b0;
			}
		}
		ADBMS6830B_set_cfgr(icn, IC_Config, refon, cth_bits, gpio_bits, (bits & balancing_mask), dcto_bits, uv, ov);
	}
	ADBMS6830B_wrcfgb(FEB_NUM_IC, IC_Config);
}

bool FEB_Cell_Balancing_Status(void) {
    float min_v = FLT_MAX;
    float max_v = FLT_MIN;

    for (size_t i = 0; i < FEB_NBANKS; ++i) {
        for (size_t j = 0; j < FEB_NUM_CELL_PER_BANK; ++j) {
            const float voltage = FEB_ADBMS_GET_Cell_Voltage_V(i, j) * 1000.0f;
            const float temp = FEB_ADBMS_GET_Cell_Temperature(i, j) * 10.0f;

            if (temp >= FEB_CONFIG_CELL_SOFT_MAX_TEMP_dC) {
                return false;
            }

            if (voltage < 0) {
                continue;
            }

            if (voltage < min_v) min_v = voltage;
            if (voltage > max_v) max_v = voltage;
        }
    }

    if (max_v < 0 || min_v > 1e8f) {
        return false;
    }

    const float delta_v = max_v - min_v;
    if (delta_v >= FEB_MIN_SLIPPAGE_V) {
        return true; 
    }

    return false;    
}

void FEB_Stop_Balance(){
	for (uint8_t ic = 0; ic < FEB_NUM_IC; ic++) {
		ADBMS6830B_set_cfgr(ic, IC_Config, refon, cth_bits, gpio_bits, 0, dcto_bits, uv, ov);
	}
	ADBMS6830B_wrALL(FEB_NUM_IC, IC_Config);
	transmitCMD(ADCV|AD_DCP);
	balancing_active = false;
}

// ********************************** Error Type *********************************

uint8_t FEB_ADBMS_Get_Error_Type() {
	return ERROR_TYPE;
}

void FEB_ADBMS_Update_Error_Type(uint8_t error) {
	ERROR_TYPE = error;
}
