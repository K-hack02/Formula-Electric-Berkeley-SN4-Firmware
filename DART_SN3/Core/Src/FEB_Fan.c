// ********************************** Includes & External **********************************
#include "FEB_Fan.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim14;

extern UART_HandleTypeDef huart2;

// ********************************** Initialize **********************************

void FEB_Fan_Init(void) {
	FEB_Fan_PWM_Init();
	FEB_Fan_All_Speed_Set(40 * .1); // starts at 10% duty cycle
	FEB_Fan_TACH_Init();
}

// ********************************** CAN **********************************

void FEB_Fan_CAN_Msg_Process(uint8_t* FEB_CAN_Rx_Data) {
	FEB_Fan_1_Speed_Set(FEB_CAN_Rx_Data[0]);
	FEB_Fan_2_Speed_Set(FEB_CAN_Rx_Data[1]);
	FEB_Fan_3_Speed_Set(FEB_CAN_Rx_Data[2]);
	FEB_Fan_4_Speed_Set(FEB_CAN_Rx_Data[3]);
	FEB_Fan_5_Speed_Set(FEB_CAN_Rx_Data[4]);
}

// ********************************** PWM **********************************
void FEB_Fan_PWM_Init(void) {
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); //fan 1
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); //fan 2
	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); //fan 3
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2); //fan 4
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1); //fan 5
}

void FEB_Fan_TACH_Init(void) {
	HAL_TIM_IC_Start_IT(&htim14, TIM_CHANNEL_1); // fan 1
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3); // fan 2
	HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_4); // fan 3
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1); // fan 4
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_2); // fan 5
}

void FEB_Fan_All_Speed_Set(uint8_t speed) {
	FEB_Fan_1_Speed_Set(speed);
	FEB_Fan_2_Speed_Set(speed);
	FEB_Fan_3_Speed_Set(speed);
	FEB_Fan_4_Speed_Set(speed);
	FEB_Fan_5_Speed_Set(speed);
}

void FEB_Fan_1_Speed_Set(uint8_t speed) {
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, speed);
}

void FEB_Fan_2_Speed_Set(uint8_t speed) {
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, speed);
}

void FEB_Fan_3_Speed_Set(uint8_t speed) {
	__HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, speed);
}

void FEB_Fan_4_Speed_Set(uint8_t speed) {
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, speed);
}

void FEB_Fan_5_Speed_Set(uint8_t speed) {
	__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, speed);
}
