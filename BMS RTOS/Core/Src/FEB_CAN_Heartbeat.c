// ********************************** Includes & Externs *************************

#include "FEB_CAN_Heartbeat.h"
#include "FEB_Const.h"

#include <stdatomic.h>

// ********************************** Structs ************************************

typedef struct {
	atomic_uint          	seq;
    atomic_uint_least32_t 	last_received;
	atomic_uint_least8_t  	FAck;			// Failed Acknowledgments (external code may increment)
    atomic_uint_least8_t  	LaOn;			// Count of CAN SM reports since last struct update
    atomic_uint_least8_t  	initialized;	// >0 means we've seen a valid heartbeat
} FEB_CAN_DEV;

// ********************************** Variables **********************************

static FEB_CAN_DEV FEB_CAN_NETWORK[DEV_IND_ALL]={0};


// ********************************** Static Functions ***************************

static inline void hb_write_begin(FEB_CAN_DEV *d){
    atomic_fetch_add_explicit(&d->seq, 1u, memory_order_relaxed);  // -> odd
}
static inline void hb_write_end(FEB_CAN_DEV *d){
    atomic_fetch_add_explicit(&d->seq, 1u, memory_order_release);  // -> even
}

static inline void hb_read_consistent(FEB_CAN_DEV *d, uint32_t *last_received, uint8_t *FAck, uint8_t *LaOn, uint8_t *initialized) {
    unsigned s0, s1;
    do {
        s0 = atomic_load_explicit(&d->seq, memory_order_acquire);
        if (s0 & 1u) continue;
        *last_received    = atomic_load_explicit(&d->last_received,     memory_order_relaxed);
        *FAck       = atomic_load_explicit(&d->FAck,        memory_order_relaxed);
        *LaOn       = atomic_load_explicit(&d->LaOn,        memory_order_relaxed);
        *initialized= atomic_load_explicit(&d->initialized, memory_order_relaxed);
        s1 = atomic_load_explicit(&d->seq, memory_order_acquire);
    } while (s0 != s1 || (s1 & 1u));
}

// ********************************** Functions **********************************

void FEB_CAN_Heartbeat_Init(void) {
    for (uint8_t i = 0; i < (uint8_t)DEV_IND_ALL; ++i) {
		atomic_store_explicit(&FEB_CAN_NETWORK[i].seq, 0u, memory_order_relaxed);
        atomic_store_explicit(&FEB_CAN_NETWORK[i].last_received, 0u, memory_order_relaxed);
        atomic_store_explicit(&FEB_CAN_NETWORK[i].FAck, (uint8_t)FEB_FACK_THRESHOLD, memory_order_relaxed);
        atomic_store_explicit(&FEB_CAN_NETWORK[i].LaOn, 0u, memory_order_relaxed);
        atomic_store_explicit(&FEB_CAN_NETWORK[i].initialized, (uint8_t)INITIALIZED_THRESHOLD, memory_order_relaxed);
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
    hb_write_begin(d);
    atomic_store_explicit(&d->last_received, HAL_GetTick(), memory_order_relaxed);
    atomic_store_explicit(&d->FAck, 0u, memory_order_relaxed);
    atomic_store_explicit(&d->LaOn, 0u, memory_order_relaxed);
    atomic_store_explicit(&d->initialized, 0u, memory_order_relaxed);
    hb_write_end(d);

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
	return atomic_load_explicit(&FEB_CAN_NETWORK[device_index].initialized, memory_order_acquire);
}

void FEB_CAN_Heartbeat_Decrement_Initialized(FEB_DEV_INDEX device_index) {
	uint8_t curr = atomic_load_explicit(&FEB_CAN_NETWORK[device_index].initialized, memory_order_relaxed);
    if (curr > 0) {
        (void)atomic_fetch_sub_explicit(&FEB_CAN_NETWORK[device_index].initialized, 1u, memory_order_relaxed);
    }
}

uint32_t FEB_CAN_Heartbeat_Get_Last_Received(FEB_DEV_INDEX device_index) {
    return atomic_load_explicit(&FEB_CAN_NETWORK[device_index].last_received, memory_order_acquire);
}

void FEB_CAN_Heartbeat_Reset_Last_Received(FEB_DEV_INDEX device_index) {
	atomic_store_explicit(&FEB_CAN_NETWORK[device_index].last_received, 0u, memory_order_relaxed);
}

void FEB_CAN_Heartbeat_Increment_LaOn(FEB_DEV_INDEX device_index) {
	(void)atomic_fetch_add_explicit(&FEB_CAN_NETWORK[device_index].LaOn, 1u, memory_order_relaxed);
}

void FEB_CAN_Heartbeat_Increment_FAck(FEB_DEV_INDEX device_index) {
	(void)atomic_fetch_add_explicit(&FEB_CAN_NETWORK[device_index].FAck, 1u, memory_order_relaxed);
}
