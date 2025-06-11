 #ifndef INC_FEB_CAN_AUTO_H_
 #define INC_FEB_CAN_AUTO_H_
 
 #include <string.h>
 #include <stdbool.h>
 #include "FEB_CAN_Library_SN4/gen/feb_can.h"
 #include "FEB_CAN_BMS.h"
 
 extern bool auto_on;
 extern uint16_t torque; 

 uint8_t FEB_CAN_AUTO_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);
 void FEB_CAN_AUTO_Store_Msg(CAN_RxHeaderTypeDef* pHeader, uint8_t *RxData);
 
 #endif /* INC_FEB_CAN_AUTO_H_ */
 
