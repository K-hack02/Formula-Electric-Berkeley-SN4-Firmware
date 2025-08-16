#ifndef INC_FEB_CELL_TEMP_LUT_H_
#define INC_FEB_CELL_TEMP_LUT_H_

// ********************************** Includes ***********************************

#include <stdint.h>

// ********************************** Macros *************************************

#define FEB_CELL_TEMP_LUT_MIN_VOLT_mV 1300u
#define FEB_CELL_TEMP_LUT_MAX_VOLT_mV 2380u

// ********************************** Functions **********************************

int16_t FEB_Cell_Temp_LUT_Get_Temp_100mC(uint16_t voltage_mV);

#endif /* INC_FEB_CELL_TEMP_LUT_H_ */
