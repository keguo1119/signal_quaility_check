#ifndef __WG_GSM_OAM_H__
#define __WG_GSM_OAM_H__


void gsm_oam_power_set(gsm_state_t *gsm_state, uint8_t power);
void gsm_oam_mobile_power_set(gsm_state_t *gsm_state, uint16_t power);
void gsm_oam_reset(gsm_state_t *gsm_state);

void gsm_oam_inner_amp_set(gsm_state_t *gsm_state, uint8_t flag);
void gsm_oam_rf_set(gsm_state_t *gsm_state, uint8_t flag);
void gsm_oam_lbs_set(gsm_state_t *gsm_state, uint8_t flag);
void gsm_oam_alarm_set(gsm_state_t *gsm_state, uint8_t *imsi);
void gsm_oam_whitelist_set(gsm_state_t *gsm_state, uint8_t *imsi);
void gsm_oam_sms_send(gsm_state_t *gsm_state, uint8_t *imsi, uint8_t *phone, uint8_t *sms);
void gsm_oam_time_set(gsm_state_t *gsm_state);
void gsm_oam_para_set(gsm_state_t *gsm_state, uint16_t arfcn);

#endif 