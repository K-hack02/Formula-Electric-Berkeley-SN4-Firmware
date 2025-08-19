#ifndef INC_FEB_MAIN_H_
#define INC_FEB_MAIN_H_

// ********************************** Functions **********************************

void FEB_Task_SM(void);
void FEB_Task_IVT(void);
void FEB_Task_CAN(void);
void FEB_Task_UART(void);
void FEB_Main_Setup(void);
void FEB_Task_ADBMS(void);
void FEB_Task_Balance(void);
void FEB_Task_Charging(void);
void FEB_Task_CAN_Charging(void);

#endif /* INC_FEB_MAIN_H_ */
