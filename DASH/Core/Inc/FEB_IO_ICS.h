#ifndef INC_FEB_IO_ICS_H_
#define INC_FEB_IO_ICS_H_

// ********************************** Includes **********************************

#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include <stdint.h>
#include "lvgl.h"

// ********************************** Functions *********************************

void     FEB_IO_ICS_Init(void);
void     FEB_IO_ICS_Reset_All(void);
void     FEB_IO_ICS_Loop(void);
uint8_t  FEB_IO_ICS_Read_All(void);

uint8_t  set_n_bit(uint8_t num, uint8_t n, uint8_t bit_value);
void     uint8_to_binary_string(uint8_t value, char *binary_string);

void     disable_r2d(void);
void     enable_r2d(void);
bool     is_r2d(void);

#endif /* INC_FEB_IO_ICS_H_ */
