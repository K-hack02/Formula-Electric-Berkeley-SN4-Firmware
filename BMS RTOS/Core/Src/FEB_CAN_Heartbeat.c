// ********************************** Includes & Externs *************************

#include "FEB_CAN_Heartbeat.h"
#include "FEB_Const.h"

#include "cmsis_os.h"

// ********************************** Structs ************************************

typedef struct {
	volatile uint32_t     	seq;
    volatile uint32_t  		last_received;
	volatile uint8_t  		FAck;			// Failed Acknowledgments (external code may increment)
    volatile uint8_t  		LaOn;			// Count of CAN SM reports since last struct update
    volatile uint8_t  		initialized;	// >0 means we've seen a valid heartbeat
} FEB_CAN_DEV;

// ********************************** Variables **********************************

static FEB_CAN_DEV FEB_CAN_NETWORK[DEV_IND_ALL]={0};

// ********************************** Static Functions ***************************

static inline UBaseType_t hb_write_begin_from_isr(FEB_CAN_DEV *d){
	UBaseType_t ux_status = taskENTER_CRITICAL_FROM_ISR();
    d->seq++;
    return ux_status;
}
static inline void hb_write_end_from_isr(UBaseType_t ux_status, FEB_CAN_DEV *d){
	d->seq++;
    taskEXIT_CRITICAL_FROM_ISR(ux_status);
}

static inline void hb_read_consistent(FEB_CAN_DEV *d, uint32_t *last_received, uint8_t *FAck, uint8_t *LaOn, uint8_t *initialized) {
    unsigned s0, s1;
    do {
        s0 = d->seq;
        if (s0 & 1u) continue;
        *last_received = d->last_received;
        *FAck          = d->FAck;
        *LaOn          = d->LaOn;
        *initialized   = d->initialized;
        s1 = d->seq;
    } while (s0 != s1 || (s1 & 1u));
}

static inline uint8_t hb_read_u8_consistent(FEB_CAN_DEV *d, volatile uint8_t *field) {
    uint32_t s0, s1;
    uint8_t  v;
    do {
        s0 = d->seq;
        if (s0 & 1u) continue;
        v  = *field;
        s1 = d->seq;
    } while (s0 != s1 || (s1 & 1u));
    return v;
}

static inline uint32_t hb_read_u32_consistent(FEB_CAN_DEV *d, volatile uint32_t *field) {
    uint32_t s0, s1, v;
    do {
        s0 = d->seq;
        if (s0 & 1u) continue;
        v  = *field;
        s1 = d->seq;
    } while (s0 != s1 || (s1 & 1u));
    return v;
}

// ********************************** Functions **********************************

void FEB_CAN_Heartbeat_Init(void) {
    for (uint8_t i = 0; i < (uint8_t)DEV_IND_ALL; ++i) {
		FEB_CAN_NETWORK[i].seq          = 0u;
        FEB_CAN_NETWORK[i].last_received= 0u;
        FEB_CAN_NETWORK[i].FAck         = (uint8_t)FEB_FACK_THRESHOLD;
        FEB_CAN_NETWORK[i].LaOn         = 0u;
        FEB_CAN_NETWORK[i].initialized  = (uint8_t)INITIALIZED_THRESHOLD;
    }
}

uint8_t FEB_CAN_Heartbeat_Filter_Config(CAN_HandleTypeDef* hcan, uint8_t FIFO_assignment, uint8_t filter_bank) {
	uint16_t ids[] = {FEB_CAN_PCU_HEARTBEAT_FRAME_ID, FEB_CAN_DASH_HEARTBEAT_FRAME_ID, FEB_CAN_LVPDB_HEARTBEAT_FRAME_ID, 
						FEB_CAN_DCU_HEARTBEAT_FRAME_ID, FEB_CAN_FRONT_SENSOR_HEARTBEAT_MESSAGE_FRAME_ID, 
						FEB_CAN_REAR_SENSOR_HEARTBEAT_MESSAGE_FRAME_ID
					};

	for (uint8_t i = 0; i < sizeof(ids) / sizeof(ids[0]); i++) {
		CAN_FilterTypeDef filter_config;

	    // Standard CAN - 2.0A - 11 bit
	    filter_config.FilterActivation = CAN_FILTER_ENABLE;
		filter_config.FilterBank = filter_bank;
		filter_config.FilterFIFOAssignment = FIFO_assignment;
		filter_config.FilterIdHigh = ids[i] << 5;
		filter_config.FilterIdLow = 0;
		filter_config.FilterMaskIdHigh = 0xFFE0;
		filter_config.FilterMaskIdLow = 0;
		filter_config.FilterMode = CAN_FILTERMODE_IDMASK;
		filter_config.FilterScale = CAN_FILTERSCALE_32BIT;
		filter_config.SlaveStartFilterBank = 27;
	    filter_bank++;

		if (HAL_CAN_ConfigFilter(hcan, &filter_config) != HAL_OK) {
		}
	}
	return filter_bank;
}

void FEB_CAN_Heartbeat_Store_Msg(CAN_RxHeaderTypeDef *rx_header, uint8_t rx_data[]) {
	FEB_DEV_INDEX device;

	switch(rx_header->StdId) {
		case 0:
	    	device = 0;
			break;
		case FEB_CAN_DASH_HEARTBEAT_FRAME_ID:
			device = DEV_IND_DASH;
			break;
		case FEB_CAN_PCU_HEARTBEAT_FRAME_ID: 
			device = DEV_IND_PCU;
			break;
		case FEB_CAN_LVPDB_HEARTBEAT_FRAME_ID:
			device = DEV_IND_LVPDB;
			break;
		case FEB_CAN_DCU_HEARTBEAT_FRAME_ID:
			device = DEV_IND_DCU;
			break;
		case FEB_CAN_FRONT_SENSOR_HEARTBEAT_MESSAGE_FRAME_ID:
			device = DEV_IND_FSN;
			break;
		case FEB_CAN_REAR_SENSOR_HEARTBEAT_MESSAGE_FRAME_ID:
			device = DEV_IND_RSN;
			break;
		default: 
			return;
	}

	FEB_CAN_DEV *d = &FEB_CAN_NETWORK[device];
    UBaseType_t ux_status = hb_write_begin_from_isr(d);
	d->last_received = HAL_GetTick();
    d->FAck          = 0;
    d->LaOn          = 0;
    d->initialized   = 0;
	hb_write_end_from_isr(ux_status, d);
}

FEB_DEV_STATUS FEB_GetStatus(FEB_DEV_INDEX idx) {
    uint32_t last_received; 
	uint8_t fack; 
	uint8_t laon;
	uint8_t init;
	
    hb_read_consistent(&FEB_CAN_NETWORK[idx], &last_received, &fack, &laon, &init);

    if (init > 0) {
		return INITIALIZED;
	}

    if (fack < (uint8_t)FEB_FACK_THRESHOLD) {
		return CONNECTED;
	}

    return DISCONNECTED;
}


FEB_DEV_STATUS FEB_COMBINED_STATUS() {
	FEB_DEV_STATUS dash = FEB_GetStatus(DEV_IND_DASH);
	if (dash == INITIALIZED) return INITIALIZED;

	FEB_DEV_STATUS pcu = FEB_GetStatus(DEV_IND_PCU);
	if (pcu == INITIALIZED) return INITIALIZED;

	if (dash == DISCONNECTED || pcu == DISCONNECTED) return DISCONNECTED;

	return CONNECTED;
}

uint8_t FEB_CAN_Heartbeat_Get_Initialized(FEB_DEV_INDEX device_index) {
	FEB_CAN_DEV *d = &FEB_CAN_NETWORK[device_index];
	return (uint8_t)hb_read_u8_consistent(d, &d->initialized);
}

uint32_t FEB_CAN_Heartbeat_Get_Last_Received(FEB_DEV_INDEX device_index) {
	FEB_CAN_DEV *d = &FEB_CAN_NETWORK[device_index];
	return (uint32_t)hb_read_u32_consistent(d, &d->last_received);
}

void FEB_CAN_Heartbeat_Decrement_Initialized(FEB_DEV_INDEX device_index) {
	taskENTER_CRITICAL();
    if (FEB_CAN_NETWORK[device_index].initialized > 0) {
        FEB_CAN_NETWORK[device_index].initialized--;
    }
    taskEXIT_CRITICAL();
}

void FEB_CAN_Heartbeat_Reset_Last_Received(FEB_DEV_INDEX device_index) {
	taskENTER_CRITICAL();
    FEB_CAN_NETWORK[device_index].last_received = 0;
    taskEXIT_CRITICAL();
}

void FEB_CAN_Heartbeat_Increment_LaOn(FEB_DEV_INDEX device_index) {
	taskENTER_CRITICAL();
    FEB_CAN_NETWORK[device_index].LaOn++;
    taskEXIT_CRITICAL();
}

void FEB_CAN_Heartbeat_Increment_FAck(FEB_DEV_INDEX device_index) {
	taskENTER_CRITICAL();
    FEB_CAN_NETWORK[device_index].FAck++;
    taskEXIT_CRITICAL();
}
