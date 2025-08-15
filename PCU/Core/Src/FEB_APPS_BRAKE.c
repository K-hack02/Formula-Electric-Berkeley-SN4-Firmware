// ********************************** Includes & Externs ***********************

#include "FEB_APPS_BRAKE.h"
#include "FEB_CAN.h"
#include "FEB_Const.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef  hadc1;
extern UART_HandleTypeDef huart2;

// ********************************** Variables ********************************

static float    normalized_acc   = 0.0f;
static float    brake_percent	 = 0.0f;
static uint16_t psi1             = 0u;
static uint16_t psi2             = 0u;
static uint16_t acc_pedal_1		 = 0u;
static uint16_t acc_pedal_2		 = 0u;
static bool     isImpl           = false;

// ********************************** Functions ********************************

void FEB_Set_Normalized_Acc_0(){
	normalized_acc = 0.0f;
}

float FEB_Get_Normalized_Acc(){
	return normalized_acc;
}

float FEB_Get_Brake_Percent() {
	return brake_percent;
}

void FEB_Update_Normalized_Acc(){
	normalized_acc = FEB_Normalized_Acc_Pedals();
}

void FEB_Update_Brake_Percent() {
	brake_percent = FEB_Brake_Pedals_Percent();
	psi1 = FEB_Calculate_PSI1();
	psi2 = FEB_Calculate_PSI2();

}

uint16_t FEB_Read_ADC(uint32_t channel){
	ADC_ChannelConfTypeDef sConfig={0};
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		// Optional: Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    return (uint16_t)HAL_ADC_GetValue(&hadc1);
}

float FEB_Normalized_Acc_Pedals() {

	uint16_t acc_pedal_1 = FEB_Read_ADC(ACC_PEDAL_1);
	uint16_t acc_pedal_2 = FEB_Read_ADC(ACC_PEDAL_2);

	float ped1_normalized = (acc_pedal_1 - ACC_PEDAL_1_START) / (1.0f * ACC_PEDAL_1_END - ACC_PEDAL_1_START);
    float ped2_normalized = (acc_pedal_2 - ACC_PEDAL_2_START) / (1.0f * ACC_PEDAL_2_END - ACC_PEDAL_2_START);

	if (acc_pedal_1 > ACC_PEDAL_1_END + 20 || acc_pedal_1 < ACC_PEDAL_1_START - 20 || 
		acc_pedal_2 > ACC_PEDAL_2_END + 20 || acc_pedal_2 < ACC_PEDAL_2_START - 20) {
		
		isImpl = true;
		return 0.0;
	}

	if(fabsf(ped1_normalized - ped2_normalized) > 0.1 ){
		isImpl = true;
		return 0.0;
	}

	
	float final_normalized = 0.5f * (ped1_normalized + ped2_normalized); 

	if (final_normalized < 0.0f) {
        final_normalized = 0.0f;
    } else if (final_normalized > 1.0f) {
        final_normalized = 1.0f;
    }

	FEB_Update_Brake_Percent();
	if (brake_percent > 20.0f && normalized_acc > 0.1) {
		isImpl = true;
	}

	if (isImpl && final_normalized < 0.05f && brake_percent < 0.15f) {
		isImpl = false;
	}

	if (!isImpl) {
		return final_normalized;
	} else {
		return 0.0f;
	}
}

float FEB_Brake_Pedals_Percent() {
	
	uint16_t brake_pres =  FEB_Read_ADC(BRAKE_IN);   
	float final_normalized = (brake_pres - BRAKE_PRESSURE_START) / (float)(BRAKE_PRESSURE_END - BRAKE_PRESSURE_START);

    if (final_normalized < 0.0f) {
        final_normalized = 0.0f;
    } else if (final_normalized > 1.0f) {
        final_normalized = 1.0f;
    }
    
    brake_percent = final_normalized * 100.0f;

    return brake_percent;
}

uint16_t FEB_Calculate_PSI1() {
	uint16_t raw       = FEB_Read_ADC(BRAKE_PRESS_1);
    float    voltage_1 = raw / (3.3f / (3.3f + 1.69f));
    float    psi       = ((voltage_1 - 500.0f) / 4000.0f) * 1000.0f;
    return (uint16_t)psi;
}

uint16_t FEB_Calculate_PSI2() {
	uint16_t raw       = FEB_Read_ADC(BRAKE_PRESS_2);
    float    voltage_2 = raw / (3.3f / (3.3f + 1.69f));
    float    psi       = ((voltage_2 - 500.0f) / 4000.0f) * 1000.0f;
    return (uint16_t)psi;
}


void FEB_CAN_Brake() {
	// Data: [0] = brake% (0..100), [1..2] = psi1, [3..4] = psi2
    uint8_t data[5];

	data[0] = brake_percent;
    data[1] = (uint8_t)(psi1 & 0xFF);
    data[2] = (uint8_t)((psi1 >> 8) & 0xFF);
    data[3] = (uint8_t)(psi2 & 0xFF);
    data[4] = (uint8_t)((psi2 >> 8) & 0xFF);

	(void)FEB_CAN_Send_Std(FEB_CAN_BRAKE_FRAME_ID, data, (uint8_t)sizeof(data));
}

void FEB_CAN_APPS() {
	uint8_t data[4] = {
		(uint8_t)(acc_pedal_1 & 0xFF), (uint8_t)((acc_pedal_1 >> 8) & 0xFF),
		(uint8_t)(acc_pedal_2 & 0xFF), (uint8_t)((acc_pedal_2 >> 8) & 0xFF)
  	};

	(void)FEB_CAN_Send_Std(FEB_CAN_PCU_RAW_ACC_FRAME_ID, data, (uint8_t)sizeof(data));
}

void FEB_Read_All_Pedals(void) {
    // Brake
    {
        uint16_t raw = FEB_Read_ADC(BRAKE_IN);
        float pct = 0.03256f * raw - 13.4f;
        if (pct > 100.0f) {
			pct = 100.0f;
		}
		if (pct < 0.0f) {
			pct = 0.0f;
		}

        int int_part = (int)pct;
        int thou_part = (int)((pct - int_part) * 1000.0f + 0.5f);
        
		char buf[128];
        sprintf(buf, "[SENSOR] Brake Position RAW: %u\n", raw);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
        sprintf(buf, "[SENSOR] Brake Position: %d.%03d%%\n", int_part, thou_part);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
    }

    // Accelerator Pedal 1
    {
        uint16_t raw = FEB_Read_ADC(ACC_PEDAL_1);
        float pct = 0.03256f * raw - 13.4f;
        if (pct > 100.0f) {
			pct = 100.0f;
		}
		if (pct < 0.0f) {
			pct = 0.0f;
		}

        int int_part = (int)pct;
        int thou_part = (int)((pct - int_part) * 1000.0f + 0.5f);

        char buf[128];
        sprintf(buf, "[SENSOR] Accelerator 1 Position RAW: %u\n", raw);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
        sprintf(buf, "[SENSOR] Accelerator 1 Position: %d.%03d%%\n", int_part, thou_part);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
    }

    // Accelerator Pedal 2
    {
        uint16_t raw = FEB_Read_ADC(ACC_PEDAL_2);
        float pct = 0.36437f * raw - 114.8547f;
        if (pct > 100.0f) {
			pct = 100.0f;
		}
		if (pct < 0.0f) {
			pct = 0.0f;
		}
        int int_part = (int)pct;
        int thou_part = (int)((pct - int_part) * 1000.0f + 0.5f);

        char buf[128];
        sprintf(buf, "[SENSOR] Accelerator 2 Position RAW: %u\n", raw);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
        sprintf(buf, "[SENSOR] Accelerator 2 Position: %d.%03d%%\n", int_part, thou_part);
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, strlen(buf), HAL_MAX_DELAY);
    }
}


