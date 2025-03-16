#ifndef INC_FEB_MAIN_H_
#define INC_FEB_MAIN_H_

#include "FEB_Fan.h"
#include "FEB_CAN.h"
#include <string.h>
#include "Main.h"

void FEB_Init(void);
void FEB_Main_Loop(void);
void FEB_Error_Handler();

#endif /* INC_FEB_MAIN_H_ */
