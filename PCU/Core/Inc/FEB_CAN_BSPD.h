#ifndef INC_FEB_CAN_BSPD_H_
#define INC_FEB_CAN_BSPD_H_

// ********************************** Includes **********************************

#include "stm32f4xx_hal.h"
//#include "FEB_CAN_ID.h"
#include "feb_can_dbc.h"

// ********************************** Functions **********************************


void FEB_CAN_BSPD_checkReset();
void FEB_CAN_BSPD_Transmit();

#endif /* INC_FEB_CAN_BSPD_H_ */
