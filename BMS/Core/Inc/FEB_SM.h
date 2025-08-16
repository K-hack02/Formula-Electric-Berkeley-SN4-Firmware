#ifndef INC_FEB_SM_H_
#define INC_FEB_SM_H_

// ********************************** Includes ***********************************

#include <stdbool.h>
#include <stdint.h>
#include "FEB_Config.h"

// ********************************** Functions **********************************

void        FEB_SM_Init(void);
FEB_SM_ST_t FEB_SM_Get_Current_State(void);
void        FEB_SM_Transition(FEB_SM_ST_t next_state);
void        FEB_SM_Process(void);

#endif /* INC_FEB_SM_H_ */
