// ********************************** Includes & Externs *************************

#include "FEB_UART_Transmit.h"
#include "FEB_SM.h" 
#include "FEB_HW.h"
#include "FEB_CAN_IVT.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"

extern UART_HandleTypeDef huart2;
extern accumulator_t      FEB_ACC;

// ********************************** Functions **********************************

static uint8_t counter = 0;
static char UART_line[256];

// ********************************** Functions **********************************

void FEB_UART_Transmit_Process(void) {
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "Counter: %u\r\n", (unsigned)counter++);
    if (n > 0) {
        HAL_UART_Transmit(&huart2, (uint8_t*)buf, (uint16_t)n, 100);
    }
}

void FEB_ADBMS_UART_Transmit() {
	int NUMLINES=5;
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank++) {
		char UART_line[NUMLINES][32*FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK];
		int offset[NUMLINES];
		offset[0]=sprintf((char*)(UART_line[0]),"|Bnk %d|",bank+1);
		offset[1]=sprintf((char*)(UART_line[1]),"|Vlt C|");
		offset[2]=sprintf((char*)(UART_line[2]),"|Vlt S|");
		offset[3]=sprintf((char*)(UART_line[3]),"|Tmp 1|");
		offset[4]=sprintf((char*)(UART_line[4]),"|DsChg|");
		//offset[4]=sprintf((char*)(UART_line[5]),"|PWM  |");

		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK; cell++) {
			offset[0]+=sprintf(((char*)(UART_line[0]) + offset[0]), (cell>=9)?"Cell  %d|":"Cell   %d|",cell+1);
			float CV =FEB_ACC.banks[bank].cells[cell].voltage_V;
			offset[1]+=sprintf(((char*)(UART_line[1]) + offset[1]), CV>0?"%.6f|":"%.5f|",CV);
			float SV =FEB_ACC.banks[bank].cells[cell].voltage_S;
			offset[2]+=sprintf(((char*)(UART_line[2]) + offset[2]), SV>0?"%.6f|":"%.5f|",SV);
			offset[3]+=sprintf(((char*)(UART_line[3]) + offset[3]), "%.5f|",FEB_ACC.banks[bank].temp_sensor_readings_V[cell]);
			offset[4]+=sprintf(((char*)(UART_line[4]) + offset[4]), FEB_ACC.banks[bank].cells[cell].discharging==1?"True  |":"False |");
			//offset[5]+=sprintf(((char*)(UART_line[4]) + offset[4]), "%X|",FEB_ACC.banks[bank].temp_sensor_readings_V[cell+16]);
		}

		offset[NUMLINES-1]+=sprintf(((char*)(UART_line[NUMLINES-1]) + offset[NUMLINES-1]), "\n\r");
		char Bank_line[NUMLINES*32*FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK];
		int index =0;
		for(int line=0;line<NUMLINES;line++){
			offset[line]+=sprintf(((char*)(UART_line[line]) + offset[line]), "\n\r") ;
			index+=sprintf(((char*)Bank_line)+index,UART_line[line]);
		}

		if(bank+1==FEB_NBANKS)index+=sprintf(((char*)Bank_line)+index,"Total voltage: %f \n\r",FEB_ACC.total_voltage_V);
		HAL_UART_Transmit(&huart2, (uint8_t*) Bank_line, index+1, 100);
	}
}

void FEB_MONITOR_UART_Transmit(){
	for (uint8_t bank = 0; bank < FEB_NBANKS; bank++) {
		for (uint8_t cell = 0; cell < FEB_NUM_CELLS_PER_IC*FEB_NUM_ICPBANK; cell++) {
			int i = 0;
			i+=sprintf(((char*)(UART_line)),"cell %d %d",
					bank,
					cell
				);
			i+=sprintf(((char*)(UART_line)+i)," %.6f",
					FEB_ACC.banks[bank].cells[cell].voltage_V);
			i+=sprintf(((char*)(UART_line)+i)," %.6f",
					FEB_ACC.banks[bank].cells[cell].voltage_S);
			i+=sprintf(((char*)(UART_line)+i)," %.6f",
					FEB_ACC.banks[bank].temp_sensor_readings_V[cell]);
			i+=sprintf(((char*)(UART_line)+i)," %s\n",
					((FEB_ACC.banks[bank].cells[cell].discharging)==1?"true":"false"));

			HAL_UART_Transmit(&huart2, (uint8_t*) UART_line, strlen(UART_line), 100);
		}
	}

	sprintf(((char*)(UART_line)),"relay %s %s %s\n",
			FEB_PIN_RD(PN_AIRP_SENSE) == 0 ? "true":"false",
			FEB_PIN_RD(PN_AIRM_SENSE) == 0 ? "true":"false",
			(FEB_PIN_RD(PN_PC_REL) == 0 ? "true":"false")
			);

	HAL_UART_Transmit(&huart2, (uint8_t*) UART_line, strlen(UART_line), 100);

}

void FEB_SM_UART_Transmit(void) {
	if (!FEB_SM_ST_DEBUG)
		return;

	FEB_SM_ST_t state = FEB_SM_Get_Current_State();
	char* state_str;
	switch(state){
		case FEB_SM_ST_BOOT:
			state_str = "State: Boot\n\r";
			break;
		case FEB_SM_ST_LV:
			state_str = "State: LV\n\r";
			break;
		case FEB_SM_ST_HEALTH_CHECK:
			state_str = "State: ESC\n\r";
			break;
		case FEB_SM_ST_PRECHARGE:
			state_str = "State: Precharge\n\r";
			break;
		case FEB_SM_ST_ENERGIZED:
			state_str = "State: energized\n\r";
			break;
		case FEB_SM_ST_DRIVE:
			state_str = "State: drive\n\r";
			break;
		case FEB_SM_ST_FREE:
			state_str = "State: free\n\r";
			break;
		case FEB_SM_ST_CHARGING:
			state_str = "State: charging\n\r";
			break;
		case FEB_SM_ST_BALANCE:
			state_str = "State: balance\n\r";
			break;
		case FEB_SM_ST_FAULT_BMS:
			state_str = "State: fault BMS\n\r";
			break;
		case FEB_SM_ST_FAULT_BSPD:
			state_str = "State: fault BSPD\n\r";
			break;
		case FEB_SM_ST_FAULT_IMD:
			state_str = "State: fault IMD\n\r";
			break;
		case FEB_SM_ST_FAULT_CHARGING:
			state_str = "State: fault charging\n\r";
			break;
		case FEB_SM_ST_CHARGER_PRECHARGE:
			state_str = "State: charger precharge\n\r";
			break;
		case FEB_SM_ST_DEFAULT:
			state_str = "State: default\n\r";
			break;
		default:
			state_str = "State: NA\n\r";
			break;
	}

	FEB_Relay_State bms_shutdown_relay = FEB_PIN_RD(PN_SHS_IN);
	FEB_Relay_State air_plus_relay = FEB_PIN_RD(PN_PC_AIR);
	FEB_Relay_State air_plus_sense = FEB_PIN_RD(PN_AIRP_SENSE);
	FEB_Relay_State air_minus_sense = FEB_PIN_RD(PN_AIRM_SENSE);
	FEB_Relay_State precharge_relay = FEB_PIN_RD(PN_PC_REL);
	FEB_Relay_State bms_shutdown_enable = FEB_PIN_RD(PN_BMS_SHUTDOWN);
	FEB_Relay_State imd_shutdown_sense = FEB_PIN_RD(PN_SHS_IMD);

	uint32_t ivt_voltage_mV = FEB_IVT_V1_Voltage();

	static char str[512];
	sprintf(str, "%sShutdown Input: %d\tBMS Shutdown: %d\tAir Plus: %d\tAIR Plus Sense: %d\tAIR Minus Sense: %d\tPrecharge Relay: %d\tIMD Shutdown Sense: %d\n\rTractive System Voltage: %ld\n\r———————————————————————————————————————\n\r",
			state_str, bms_shutdown_relay, bms_shutdown_enable, air_plus_relay, air_plus_sense,
			air_minus_sense, precharge_relay, imd_shutdown_sense, ivt_voltage_mV);

	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
}

void FEB_IVT_V1_Transmit(){
	int32_t voltage_value = FEB_IVT_V1_Voltage();
	int32_t current_value = FEB_IVT_V1_Current();
	static char str[256];
	sprintf(str,"IVT V1 Voltage: %ld \t IVT V1 Current: %ld \n\r", voltage_value, current_value);
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 500);

}

void FEB_SM_State_Transmit(){
	FEB_SM_ST_t state = FEB_SM_Get_Current_State();
	static char str[128];

	switch(state){
		case FEB_SM_ST_BOOT:
			sprintf(str,"State: Boot\n\r");
			break;
		case FEB_SM_ST_LV:
			sprintf(str,"State: LV\n\r");
			break;
		case FEB_SM_ST_HEALTH_CHECK:
			sprintf(str,"State: ESC\n\r");
			break;
		case FEB_SM_ST_PRECHARGE:
			sprintf(str,"State: Precharge\n\r");
			break;
		case FEB_SM_ST_ENERGIZED:
			sprintf(str,"State: energized\n\r");
			break;
		case FEB_SM_ST_DRIVE:
			sprintf(str,"State: drive\n\r");
			break;
		case FEB_SM_ST_FREE:
			sprintf(str,"State: free\n\r");
			break;
		case FEB_SM_ST_CHARGING:
			sprintf(str,"State: charging\n\r");
			break;
		case FEB_SM_ST_BALANCE:
			sprintf(str,"State: balance\n\r");
			break;
		case FEB_SM_ST_FAULT_BMS:
			sprintf(str,"State: fault BMS\n\r");
			break;
		case FEB_SM_ST_FAULT_BSPD:
			sprintf(str,"State: fault BSPD\n\r");
			break;
		case FEB_SM_ST_FAULT_IMD:
			sprintf(str,"State: fault IMD\n\r");
			break;
		case FEB_SM_ST_FAULT_CHARGING:
			sprintf(str,"State: fault charging\n\r");
			break;
		case FEB_SM_ST_DEFAULT:
			sprintf(str,"State: default\n\r");
			break;
		default:
			sprintf(str,"State: NA\n\r");
			break;
	}
	HAL_UART_Transmit(&huart2, (uint8_t*) str, strlen(str), 100);
}

void FEB_Transmit_AIR_Status(){
	static char str1[128];
	static char str2[128];
	static char str3[128];
	static char str4[128];
	static char str5[128];
	static char str6[128];

	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_4)){
		sprintf(str1,"AIRMINUS SET\n\r");
	}else{
		sprintf(str1,"AIRMINUS RESET\n\r");
	}


	if(HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_5)){
		sprintf(str2,"AIRPLUS SET\n\r");
	} else {
		sprintf(str2,"AIRPLUS RESET\n\r");
	}

	sprintf(str3,"BMS SHUTDOWN: %d\n\r", FEB_PIN_RD(PN_BMS_SHUTDOWN));
	sprintf(str4,"IMD SHUTDOWN: %d\n\r", FEB_PIN_RD(PN_SHS_IMD));
	sprintf(str5,"RELAY STATE: %d\n\r", FEB_PIN_RD(PN_PC_REL));

	sprintf(str6,"——————————————————————————————————————\n\r");

	HAL_UART_Transmit(&huart2, (uint8_t*) str1, strlen(str1), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str2, strlen(str2), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str5, strlen(str5), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str3, strlen(str3), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str4, strlen(str4), 100);
	HAL_UART_Transmit(&huart2, (uint8_t*) str6, strlen(str6), 100);
}
