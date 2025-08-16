#ifndef INC_FEB_APPS_BRAKE_H_
#define INC_FEB_APPS_BRAKE_H_

// ********************************** Includes *********************************

#include <stdint.h>
#include <stdbool.h>

// ********************************** Functions *********************************


uint16_t FEB_Read_ADC(uint32_t channel);

float    FEB_Get_Normalized_Acc(void);
void     FEB_Set_Normalized_Acc_0(void);
void     FEB_Update_Normalized_Acc(void);
float    FEB_Normalized_Acc_Pedals(void);

float    FEB_Get_Brake_Percent(void);
void     FEB_Update_Brake_Percent(void);
float    FEB_Brake_Pedals_Percent(void);

uint16_t FEB_Calculate_PSI1(void);
uint16_t FEB_Calculate_PSI2(void);

void     FEB_CAN_Brake(void);
void     FEB_CAN_APPS(void);

void     FEB_Read_All_Pedals(void);

#endif /* INC_FEB_APPS_BRAKE_H_ */
