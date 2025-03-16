#include "FEB_main.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;

extern UART_HandleTypeDef huart2;

void FEB_Init(void){
	FEB_CAN_Init();
	FEB_Fan_Init();
}

void FEB_Main_Loop(void){
	//Fan speed set by CAN receive interrupt

//	HAL_Delay(100);
}
