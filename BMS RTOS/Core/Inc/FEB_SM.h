#ifndef INC_FEB_SM_H_
#define INC_FEB_SM_H_

// ********************************** Includes ***********************************

#include <stdbool.h>
#include <stdint.h>
#include "FEB_Config.h"
#include "cmsis_os.h"

// ********************************** Defines ************************************

#define RTOS_DELAY_MS(ms)  vTaskDelay( pdMS_TO_TICKS(ms) ? pdMS_TO_TICKS(ms) : 1 )

// ********************************** Functions **********************************

void        FEB_SM_Init(void);
FEB_SM_ST_t FEB_SM_Get_Current_State(void);
void        FEB_SM_Transition(FEB_SM_ST_t next_state);
void        FEB_SM_Process(void);

#endif /* INC_FEB_SM_H_ */
