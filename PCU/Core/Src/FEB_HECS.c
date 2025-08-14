// ********************************** Includes & Externs ***********************

#include "FEB_HECS.h"
#include "FEB_Const.h"
#include "FEB_APPS_BRAKE.h"
#include "stm32f4xx_hal.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// ********************************** Variables ********************************

bool curr_high = false;

// ********************************** Functions ********************************

void FEB_HECS_update(){
	uint16_t current_HECS = FEB_Read_ADC(HECS_SIGNAL);
	
	// uint16_t current_ACC1 = FEB_Read_ADC(ACC_PEDAL_1);
	// uint16_t current_ACC2 = FEB_Read_ADC(ACC_PEDAL_2);
	
	// uint16_t current_BRAKE1 = FEB_Read_ADC(BRAKE_PRESS_1);
	// uint16_t current_BRAKE2 = FEB_Read_ADC(BRAKE_PRESS_2);

	if (current_HECS >= HECS_CURRENT){
		curr_high = true;
	}else{
		curr_high = false;
	}
}

bool FEB_HECS_isCurrentHigh() {
	return curr_high;
}
