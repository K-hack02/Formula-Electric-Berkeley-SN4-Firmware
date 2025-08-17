#ifndef INC_FEB_CHARGE_H_
#define INC_FEB_CHARGE_H_

// ********************************** Includes ***********************************

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"

// ********************************** Functions **********************************

void    FEB_CAN_Charger_Init(void);

void    FEB_CAN_Charger_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]);
uint8_t FEB_CAN_Charger_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank);

bool    FEB_CAN_Charger_Received(void);
void    FEB_CAN_Charger_Process(void);
void    FEB_CAN_Charger_UART_Transmit(void);
void    FEB_CAN_Charger_Start_Charge(void);
void    FEB_CAN_Charger_Stop_Charge(void);
bool    FEB_CAN_Charging_Status(void);
void    FEB_CAN_Charger_Serial(void);

#endif /* INC_FEB_CHARGE_H_ */
