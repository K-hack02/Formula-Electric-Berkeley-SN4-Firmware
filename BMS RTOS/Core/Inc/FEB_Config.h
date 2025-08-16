
#ifndef INC_FEB_CONFIG_H_
#define INC_FEB_CONFIG_H_

// ********************************** Includes ***********************************

#include <stdint.h>
#include "FEB_Const.h"

// ********************************** Functions **********************************

void FEB_Config_Update(FEB_SM_ST_t current_state);
uint16_t FEB_Config_Get_Cell_Min_Voltage_mV(void);
uint16_t FEB_Config_Get_Cell_Max_Voltage_mV(void);
float FEB_Config_Get_Cell_Min_Temperature_dC(void);
float FEB_Config_Get_Cell_Max_Temperature_dC(void);
int32_t FEB_Config_Get_Pack_Min_Current_mA(void);
int32_t FEB_Config_Get_Pack_Max_Current_mA(void);

#endif /* INC_FEB_CONFIG_H_ */
