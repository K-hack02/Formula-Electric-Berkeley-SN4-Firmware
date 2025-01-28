#include "FEB_Main.h"
#include "FEB_Hw.h"

void FEB_Main_Setup(void) {

}

void FEB_Main_Loop(void) {
	FEB_Hw_LED(FEB_HW_LED_ON);
	HAL_Delay(500);

	FEB_Hw_LED(FEB_HW_LED_OFF);
	HAL_Delay(500);
}
