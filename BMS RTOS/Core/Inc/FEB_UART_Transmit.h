
#ifndef INC_FEB_UART_TRANSMIT_H_
#define INC_FEB_UART_TRANSMIT_H_

// ********************************** Includes ***********************************

#include <stdint.h>

// ********************************** Functions **********************************

void FEB_UART_Transmit_Process(void);
void FEB_ADBMS_UART_Transmit(void);
void FEB_MONITOR_UART_Transmit(void);
void FEB_SM_UART_Transmit(void);
void FEB_IVT_V1_Transmit(void);
void FEB_SM_State_Transmit(void);
void FEB_Transmit_AIR_Status(void);

#endif /* INC_FEB_UART_TRANSMIT_H_ */