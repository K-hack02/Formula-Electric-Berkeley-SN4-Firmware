// ********************************** Includes & Externs *************************

#include "FEB_CAN.h"
#include "FEB_CAN_TPS.h"
#include "TPS2482.h"
#include "FEB_CAN_Library_SN4/gen/feb_can.h"

#include <string.h>

extern I2C_HandleTypeDef hi2c1;
extern uint8_t tps2482_i2c_addresses[1];

// ********************************** Macros *************************************

#define FLOAT_TO_UINT16_T(n)		((uint16_t)(n * 1000)) // for voltage (mV)
#define FLOAT_TO_INT16_T(n)			((int16_t)(n * 1000)) // for current (mA)
#define SIGN_MAGNITUDE(n)			(int16_t)((((n >> 15) & 0x01) == 1) ? -(n & 0x7FFF) : (n & 0x7FFF)) // for current reg

// ********************************** Functions **********************************

void FEB_CAN_TPS_Transmit() {
    uint16_t voltage_raw;
    TPS2482_Poll_Bus_Voltage(&hi2c1, tps2482_i2c_addresses, (uint16_t *) &voltage_raw, 1);
    
	uint16_t current_raw;
    TPS2482_Poll_Current(&hi2c1, tps2482_i2c_addresses, (uint16_t *) &current_raw, 1);

    uint16_t voltage = FLOAT_TO_UINT16_T(voltage_raw * TPS2482_CONV_VBUS);
    int16_t current = FLOAT_TO_INT16_T(SIGN_MAGNITUDE(current_raw) * TPS2482_CURRENT_LSB_EQ((double)(5)));

	uint8_t data[4];
    memcpy(&data[0], &voltage, sizeof(uint16_t));
    memcpy(&data[2], &current, sizeof(int16_t));

    (void)FEB_CAN_Send_Std(FEB_CAN_DASH_TPS_FRAME_ID, data, sizeof(data));
}
