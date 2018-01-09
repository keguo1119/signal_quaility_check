#ifndef __WG_GSM_ENCODE_H__
#define __WG_GSM_ENCODE_H__

/////////////////////////////////////////////////////////////////
#define  ENCODE_WHITELIST_LEN    31
#define  ENCODE_SMS_LEN          30
#define  ENCODE_POWER_LEN        2
#define  ENCODE_LBS_START_LEN    17
#define  ENCODE_LBS_STOP_LEN    0
#define  ENCODE_RF_START_LEN    2
#define  ENCODE_RF_STOP_LEN     0
#define  ENCODE_RING_LEN        30
#define  ENCODE_TIME_LEN        14
/////////////////////////////////////////////////////////////////
uint32_t wg_gsm_para_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_alarm_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_whitelist_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_sms_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_power_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_rf_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_lbs_start_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_lbs_stop_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_ring_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_mobile_power_set_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_lbs_ring_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);
uint32_t wg_gsm_time_set_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);

uint32_t wg_gsm_inner_amp_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size);

#endif 