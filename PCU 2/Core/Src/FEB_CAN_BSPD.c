// ********************************** Includes & External ***********************

#include "FEB_CAN_BSPD.h"
#include "FEB_CAN.h"
#include "stm32f4xx_hal.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"

// ********************************** Variables *********************************

static uint8_t bspd_state = 0;

// ********************************** Functions *********************************

void FEB_CAN_BSPD_checkReset(){
	if (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_2)){
		bspd_state = 1;
	} else {
		bspd_state = 0;
	}
	FEB_CAN_BSPD_Transmit();
}

void FEB_CAN_BSPD_Transmit(void) {
	uint8_t data[1] = { bspd_state };
    (void)FEB_CAN_Send_Std(FEB_CAN_BSPD_STATE_FRAME_ID, data, 1);
}
