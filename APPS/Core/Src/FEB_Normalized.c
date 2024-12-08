// **************************************** Includes & External ****************************************

#include "FEB_Normalized.h"

extern CAN_HandleTypeDef hcan1;
extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;

extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint8_t FEB_CAN_Tx_Data[8];
extern CAN_TxHeaderTypeDef FEB_CAN_Tx_Header;
extern uint32_t FEB_CAN_Tx_Mailbox;

extern UART_HandleTypeDef huart2;



const uint16_t Sensor_Min = 4095.0/5.0*0.5;
const uint16_t Sensor_Max = 4095.0/5.0*4.5;

// **************************************** Configuration ****************************************


float normalized_acc;
float normalized_brake;
bool isImpl = false;

// **************************************** Functions ****************************************

uint16_t FEB_Read_ADC(uint32_t channel){
	ADC_ChannelConfTypeDef sConfig={0};
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
//	   Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	return (uint16_t)HAL_ADC_GetValue(&hadc1);

}

float FEB_Normalized_getAcc(){
	return normalized_acc;
}

void FEB_Normalized_setAcc0(){
	normalized_acc = 0.0;
}

void FEB_Read_Accel_Pedal1() {
	uint16_t accel_pedal_1_raw = FEB_Read_ADC(ACC_PEDAL_1);

	float accel_pedal_1_position = 0.03256 * accel_pedal_1_raw - 13.4;

	if (accel_pedal_1_position > 100.0) {
		accel_pedal_1_position = 100.0;
	}

	if (accel_pedal_1_position < 0.0) {
		accel_pedal_1_position = 0.0;
	}

	int accel_pedal_1_position_int1 = accel_pedal_1_position;
	float accel_pedal_1_position_frac = accel_pedal_1_position - accel_pedal_1_position_int1;
	int accel_pedal_1_position_int2 = accel_pedal_1_position_frac * 1000;

	char buf[128];
	sprintf(buf, "[SENSOR] Accelerator 1 Position RAW: %d\n", accel_pedal_1_raw);
	HAL_UART_Transmit(&huart2,(uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	char buf1[128];
	sprintf(buf1, "[SENSOR] Accelerator 1 Position: %d.%d%%\n", accel_pedal_1_position_int1, accel_pedal_1_position_int2);
	HAL_UART_Transmit(&huart2,(uint8_t *)buf1, strlen(buf1), HAL_MAX_DELAY);
}

void FEB_Read_Accel_Pedal2() {
	uint16_t accel_pedal_2_raw = FEB_Read_ADC(ACC_PEDAL_2);

	float accel_pedal_2_position = 0.36437 * accel_pedal_2_raw - 114.8547;

	if (accel_pedal_2_position > 100.0) {
		accel_pedal_2_position = 100.0;
	}

	if (accel_pedal_2_position < 0.0) {
		accel_pedal_2_position = 0.0;
	}

	int accel_pedal_2_position_int1 = accel_pedal_2_position;
	float accel_pedal_2_position_frac = accel_pedal_2_position - accel_pedal_2_position_int1;
	int accel_pedal_2_position_int2 = accel_pedal_2_position_frac * 1000;

	char buf[128];
	sprintf(buf, "[SENSOR] Accelerator 2 Position RAW: %d\n", accel_pedal_2_raw);
	HAL_UART_Transmit(&huart2,(uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	char buf1[128];
	sprintf(buf1, "[SENSOR] Accelerator 2 Position: %d.%d%%\n", accel_pedal_2_position_int1, accel_pedal_2_position_int2);
	HAL_UART_Transmit(&huart2,(uint8_t *)buf1, strlen(buf1), HAL_MAX_DELAY);
}

void FEB_Read_Brake_Pedal() {
	uint16_t brake_pedal_raw = FEB_Read_ADC(BRAKE_PEDAL);

	float brake_pedal_position = 0.03256 * brake_pedal_raw - 13.4;

	if (brake_pedal_position > 100.0) {
		brake_pedal_position = 100.0;
	}

	if (brake_pedal_position < 0.0) {
		brake_pedal_position = 0.0;
	}

	int brake_pedal_position_int1 = brake_pedal_position;
	float brake_pedal_position_frac = brake_pedal_position - brake_pedal_position_int1;
	int brake_pedal_position_int2 = brake_pedal_position_frac * 1000;

	char buf[128];
	sprintf(buf, "[SENSOR] Brake Position RAW: %d\n", brake_pedal_raw);
	HAL_UART_Transmit(&huart2,(uint8_t *)buf, strlen(buf), HAL_MAX_DELAY);

	char buf1[128];
	sprintf(buf1, "[SENSOR] Brake Position: %d.%d%%\n", brake_pedal_position_int1, brake_pedal_position_int2);
	HAL_UART_Transmit(&huart2,(uint8_t *)buf1, strlen(buf1), HAL_MAX_DELAY);
}

void FEB_Normalized_updateAcc(){
	normalized_acc = FEB_Normalized_Acc_Pedals();

	char buf[128];
	uint8_t buf_len;
	buf_len = sprintf(buf, "normalized_acc: %f\n", normalized_acc);
	HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);

}

float FEB_Normalized_Acc_Pedals() {
	// raw ADC readings of the two acc pedal sensors
	uint16_t acc_pedal_1 = FEB_Read_ADC(ACC_PEDAL_1);
	uint16_t acc_pedal_2 = FEB_Read_ADC(ACC_PEDAL_2);
	char buf[128];
	uint8_t buf_len;
	buf_len = sprintf(buf, "acc1:%d acc2:%d\n", acc_pedal_1, acc_pedal_2);
//	HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);


	// check implausibility for shorting. TODO: check if this fulfills the short circuiting rules.
	if (acc_pedal_1 < ACC_PEDAL_1_END - 20 || acc_pedal_1 > ACC_PEDAL_1_START + 20
			|| acc_pedal_2 < ACC_PEDAL_2_START - 50 || acc_pedal_2 > ACC_PEDAL_2_END + 50) {
		isImpl = true;
		return 0.0;
	}

	//convert to % travel
	// sensor 1 has positive slope
	float ped1_normalized = (acc_pedal_1 - ACC_PEDAL_1_START)/ (ACC_PEDAL_1_END - ACC_PEDAL_1_START);
	// sensor 2 has negative slope
	float ped2_normalized = (acc_pedal_2 - ACC_PEDAL_2_START) / (ACC_PEDAL_2_END - ACC_PEDAL_2_START);

	// sensor measurements mismatch by more than 10%
	if(abs(ped1_normalized - ped2_normalized) > 0.1 ){
		isImpl = true;
		return 0.0;
	}

	float final_normalized = 0.5*(ped1_normalized + ped2_normalized);


//	 Implausiblity check if both pedals are stepped
	if (normalized_brake > 0.2 && normalized_acc > 0.1) {
		isImpl = true;
	}

	// recover from implausibility if acc pedal is not 5% less
	if (final_normalized < 0.05 && isImpl) {
		isImpl = false;
	}

	if (!isImpl) {
		final_normalized = final_normalized > 1 ? 1 : final_normalized;
		final_normalized = final_normalized < 0.05 ? 0 : final_normalized;
		return final_normalized;
	} else {
		return 0.0;
	}
}

float FEB_Normalized_getBrake() {
	return normalized_brake;
}

void FEB_Normalized_update_Brake() {
	normalized_brake = FEB_Normalized_Brake_Pedals();
}

float FEB_Normalized_Brake_Pedals() {
	//TODO: This might need to change based on which sensor ends up getting used.
	uint16_t brake_pres_2 =  FEB_Read_ADC(BRAKE_PRESS_2);   //HAL_ADCEx_InjectedGetValue(&hadc1, ADC_INJECTED_RANK_1);

//	HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);

	float final_normalized = (brake_pres_2 - PRESSURE_START)/ (PRESSURE_END - PRESSURE_START);
	final_normalized = final_normalized > 1 ? 1 : final_normalized;
	final_normalized = final_normalized < 0.05 ? 0 : final_normalized;


	if (brake_pres_2 < PRESSURE_START-20) {
		return 0.0;
	}

	char buf[128];
	uint8_t buf_len;
	buf_len = sprintf(buf, "brake_Pos: %f\n", final_normalized);

	HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);

	return final_normalized;
}

void FEB_Normalized_CAN_sendBrake() {
	// Initialize transmission header
	FEB_CAN_Tx_Header.DLC = 2;
	FEB_CAN_Tx_Header.StdId = FEB_CAN_ID_APPS_BRAKE_PEDAL;
	FEB_CAN_Tx_Header.IDE = CAN_ID_STD;
	FEB_CAN_Tx_Header.RTR = CAN_RTR_DATA;
	FEB_CAN_Tx_Header.TransmitGlobalTime = DISABLE;

	// Copy data to Tx buffer. This might be incorrect. It's possible you have to do some bit shifting
//	memcpy(FEB_CAN_Tx_Data, &normalized_brake, sizeof(float));
	uint8_t converted_brake_val = (uint8_t)(normalized_brake * 100);

	FEB_CAN_Tx_Data[0] = converted_brake_val;



	//Debug to see the position of the Brake position sensor


	// Delay until mailbox available
	while (HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) == 0) {}
//
	// Add Tx data to mailbox
	if (HAL_CAN_AddTxMessage(&hcan1, &FEB_CAN_Tx_Header, FEB_CAN_Tx_Data, &FEB_CAN_Tx_Mailbox) != HAL_OK) {
		//error - shutdown
		char buf[128];
		uint8_t buf_len;
		buf_len = sprintf(buf, "CAN MESSAGE FAIL TO SEND: %f\n", normalized_brake);
		HAL_UART_Transmit(&huart2,(uint8_t *)buf, buf_len, HAL_MAX_DELAY);
	}

}




