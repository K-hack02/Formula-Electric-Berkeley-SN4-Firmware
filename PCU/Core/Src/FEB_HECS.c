// **************************************** Includes & External ****************************************

#include "FEB_HECS.h"

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;



// **************************************** Configuration ****************************************

bool currHigh = false;


// **************************************** Functions ****************************************

uint16_t FEB_Read_ADC1(uint32_t channel){
	ADC_ChannelConfTypeDef sConfig={0};
	sConfig.Channel = channel;
	sConfig.Rank = 1;
	sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;

	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
	   Error_Handler();
	}

	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 100);
	return (uint16_t)HAL_ADC_GetValue(&hadc1);

}

void FEB_HECS_update(){
	uint16_t current_value = FEB_Read_ADC1(HECS_SIGNAL);
	char buf[128];
	uint8_t buf_len;
	buf_len = sprintf(buf, "HECS Current:%d\n", current_value);
	HAL_UART_Transmit(&huart2, buf, strlen(buf), HAL_MAX_DELAY);

	if (current_value >= HECS_CURRENT){ //checks if current is above value (to be determined)
		currHigh = true;
	}else{
		currHigh = false;
	}
	FEB_HECS_indicate();
}

void FEB_HECS_indicate(){ //sends GPIO output to HECS Indicator
	if (currHigh == true){

		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); // sets GPIO PA5 to high

	}else{
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET); //sets GPIPO PA5 to low
	}
//	HAL_Delay(1000);
}
