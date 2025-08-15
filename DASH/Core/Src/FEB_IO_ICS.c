// ********************************** Includes & Externs ************************

#include "FEB_IO_ICS.h"
#include "FEB_CAN_BMS.h"
#include "FEB_CAN_PCU.h"
#include "FEB_CAN_ICS.h"

extern I2C_HandleTypeDef hi2c1;

// ********************************** Consstants ********************************

#define IOEXP_ADDR ((uint16_t) 0x20)
#define BTN_HOLD_TIME ((uint32_t) 2000)
#define RTD_BUZZER_TIME ((uint32_t) 2000)
#define RTD_BUZZER_EXIT_TIME ((uint32_t)500)

// ********************************** Variables *********************************

static uint32_t rtd_press_start_time = 0;
static uint32_t rtd_buzzer_start_time = 0;
static uint8_t  set_rtd_buzzer = 1;
static uint8_t  IO_state = 0xFF;
static uint8_t  r2d = 0;

static FEB_SM_ST_t bms_state;
static FEB_SM_ST_t prev_state;
static uint8_t  entered_drive = 0;
static uint8_t  exited_drive = 0;
static uint32_t exit_buzzer_start_time = 0;

static uint32_t datalog_press_start_time = 0;
static uint8_t  datalog_active = 0;

static uint32_t button3_start_time = 0;
static uint8_t  button3_active = 0;

static uint8_t  tssi_startup = 0;
static uint8_t  imd_startup = 0;

// ********************************** Functions *********************************

static inline void ioexp_write(uint8_t value) {
    HAL_I2C_Master_Transmit(&hi2c1, IOEXP_ADDR << 1, &value, sizeof(value), HAL_MAX_DELAY);
}

static inline void ioexp_read(uint8_t *value) {
    HAL_I2C_Master_Receive(&hi2c1, IOEXP_ADDR << 1, value, 1, HAL_MAX_DELAY);
}

void FEB_IO_ICS_Init(void) {
	uint8_t initial_io_exp_state = 0xF;
	ioexp_write(initial_io_exp_state);
	bms_state = FEB_CAN_BMS_Get_State();
}

void FEB_IO_ICS_Reset_All(void) {
    rtd_press_start_time = 0;
    rtd_buzzer_start_time = 0;
    set_rtd_buzzer = 1;
    r2d = 0;

    entered_drive = 0;
    exited_drive = 0;
    exit_buzzer_start_time = 0;

    datalog_press_start_time = 0;
    datalog_active = 0;

    button3_start_time = 0;
    button3_active = 0;
}

void FEB_IO_ICS_Loop(void) {
	uint8_t received_data = 0x00;
    ioexp_read(&received_data);

	IO_state = 0;
	uint8_t brake_pressure = FEB_CAN_PCU_Get_Brake_Percent();
	uint8_t inv_enabled = FEB_CAN_PCU_Get_Inverter_Enabled();

	prev_state = bms_state;
	bms_state = FEB_CAN_BMS_Get_State();

	if (bms_state == FEB_SM_ST_LV) {
	    FEB_IO_ICS_Reset_All();
	}

	// TSSI
	if (FEB_CAN_BMS_GET_FAULTS()) {
        if (tssi_startup) {
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
        }
    } else {
        tssi_startup = 1;
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);
    }

	// IMD Fault Indicator Light
	if (FEB_CAN_GET_IMD_FAULT()) {
        if (imd_startup) {
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_SET);
        } else {
            HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
        }
    } else {
        imd_startup = 1;
        HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);
    }

	// Buzzer Logic
	if (prev_state == FEB_SM_ST_ENERGIZED && bms_state == FEB_SM_ST_DRIVE) {
        entered_drive = 1;
    } else if (prev_state == FEB_SM_ST_DRIVE && bms_state == FEB_SM_ST_ENERGIZED) {
        exited_drive = 1;
    }

	// Button 1 - Ready-to-Drive (RTD) button
	if (received_data & (1 << 1)) {
	    if ((HAL_GetTick() - rtd_press_start_time) >= BTN_HOLD_TIME &&
	        brake_pressure >= 20 &&
	        (bms_state == FEB_SM_ST_ENERGIZED || bms_state == FEB_SM_ST_DRIVE)) {

	        if (bms_state == FEB_SM_ST_ENERGIZED) {
	            r2d = 1;
	        } else if (bms_state == FEB_SM_ST_DRIVE) {
	            r2d = 0;
	        }

	        IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	        FEB_CAN_ICS_Transmit_Button_State(IO_state);
	        rtd_press_start_time = HAL_GetTick();

	    } else {
	        IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	    }
	} else {
	    rtd_press_start_time = HAL_GetTick();
	    IO_state = (uint8_t) set_n_bit(IO_state, 1, r2d);
	}

	// Button 2 - Data Logger
	if (received_data & (1 << 2)) {
		    if ((HAL_GetTick() - datalog_press_start_time) >= BTN_HOLD_TIME) {
		    	if (datalog_active == 0){
		    		datalog_active = 1;
		    	} else {
		    		datalog_active = 0;
		    	}
		    	IO_state = (uint8_t) set_n_bit(IO_state, 2, datalog_active);
		    	datalog_press_start_time = HAL_GetTick();
		    } else {
		        IO_state = (uint8_t) set_n_bit(IO_state, 2, datalog_active);
		    }
		} else {
			datalog_press_start_time = HAL_GetTick();
		    IO_state = (uint8_t) set_n_bit(IO_state, 2, datalog_active);
		}

	// Switch 1 - (Coolant Pump)
	if ((received_data & (1<<5))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 5, 1);
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 5, 0);
	}

	// Switch 2 - Radiator Fans
	if ((received_data & (1<<6))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 6, 1);
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 6, 0);
	}

	// Switch 3 - Accumulator Fans
	if ((received_data & (1<<7))) {
		IO_state = (uint8_t) set_n_bit(IO_state, 7, 1);
	} else {
		IO_state = (uint8_t) set_n_bit(IO_state, 7, 0);
	}

	if (entered_drive == 1 && bms_state ==  FEB_SM_ST_DRIVE && inv_enabled) {
		if (rtd_buzzer_start_time == 0) {
			rtd_buzzer_start_time = HAL_GetTick();
		}
		set_rtd_buzzer = 0;
		IO_state = set_n_bit(IO_state, 0, set_rtd_buzzer);
	} else if (exited_drive == 1 && bms_state == FEB_SM_ST_ENERGIZED && !inv_enabled) {
		if (exit_buzzer_start_time == 0) {
			exit_buzzer_start_time = HAL_GetTick();
		}
		set_rtd_buzzer = 0;
		IO_state = set_n_bit(IO_state, 0, set_rtd_buzzer);
	} else {
		set_rtd_buzzer = 1;
		IO_state = set_n_bit(IO_state, 0, set_rtd_buzzer);
	}

	if (((HAL_GetTick() - rtd_buzzer_start_time) >= RTD_BUZZER_TIME && rtd_buzzer_start_time > 0) ||
			((HAL_GetTick() - exit_buzzer_start_time) >= RTD_BUZZER_EXIT_TIME && exit_buzzer_start_time > 0))
	{

		rtd_buzzer_start_time = 0;
		exit_buzzer_start_time = 0;
		entered_drive = 0;
		exited_drive = 0;
		set_rtd_buzzer = 1;
		IO_state = set_n_bit(IO_state,0, set_rtd_buzzer);
	}

	uint8_t transmit_rtd = (0b1111111 << 1) + set_rtd_buzzer;
	HAL_I2C_Master_Transmit(&hi2c1, IOEXP_ADDR << 1, &transmit_rtd, 1, HAL_MAX_DELAY);
	FEB_CAN_ICS_Transmit_Button_State(IO_state);
}

uint8_t set_n_bit(uint8_t num, uint8_t n, uint8_t bit_value) {
    if (bit_value) {
        num |=  (uint8_t)(1U << n);
    } else {
        num &= (uint8_t)~(1U << n);
    }
    return num;
}

uint8_t FEB_IO_ICS_Read_All(void) {
    uint8_t raw = 0;
    ioexp_read(&raw);
    return raw;
}

void disable_r2d(){
	r2d = 0;
}

void enable_r2d(){
	r2d = 1;
}

bool is_r2d(){
	return (r2d != 0);
}
