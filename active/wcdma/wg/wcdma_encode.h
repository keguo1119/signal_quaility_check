#ifndef __WG_WCDMA_ENCODE_H__
#define __WG_WCDMA_ENCODE_H__

/////////////////////////////////////////////////////////////////
#define  ENCODE_WHITELIST_LEN    31
#define  ENCODE_SMS_LEN          30
#define  ENCODE_POWER_LEN        1
#define  ENCODE_LBS_START_LEN    17
#define  ENCODE_LBS_STOP_LEN    0
#define  ENCODE_RF_START_LEN    1
#define  ENCODE_RF_STOP_LEN     0
#define  ENCODE_RING_LEN        30

/////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_para_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_wcdma_power_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_wcdma_rf_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_wcdma_lbs_start_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_wcdma_lbs_stop_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_wcdma_2g_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_wcdma_target2g_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size);

#endif 