#ifndef INC_FEB_UI_H_
#define INC_FEB_UI_H_

// ********************************** Includes **********************************

#include <stdint.h>
#include "FEB_CAN_BMS.h"

typedef struct {
    char * test_str;
} Screen_Info_t;

// ********************************** Functions *********************************

void FEB_UI_Init(void);
void FEB_UI_Update(void);
void FEB_UI_Set_Values(void);

void BMS_State_Set(void);
char* get_bms_state_string(FEB_SM_ST_t state);

void SOC_Set_Value(float pack_voltage_v, float min_cell_voltage_v);
uint8_t lookup_soc_from_voltage(float voltage);

void TEMP_Set_Value(float max_acc_temp_centi);
void SPEED_Set_Value(float motor_speed_rpm);
void LV_Set_Value(void);

void UI_Update_ButtonsAndSwitches(uint8_t io, uint8_t inv_enabled, FEB_SM_ST_t bms_state);

#endif /* INC_FEB_UI_H_ */
