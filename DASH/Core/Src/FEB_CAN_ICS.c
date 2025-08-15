// **************************************** Includes & Externs *******************

#include "FEB_CAN_ICS.h"
#include "FEB_CAN.h"    
#include "FEB_CAN_Library_SN4/gen/feb_can.h"                  

// **************************************** Functions ****************************

void FEB_CAN_ICS_Transmit_Button_State(uint8_t io_state) {
    uint8_t data[1] = {io_state};
    (void)FEB_CAN_Send_Std(FEB_CAN_DASH_IO_FRAME_ID, data, 1);
}
