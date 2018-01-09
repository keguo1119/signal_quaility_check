#ifndef __WG_WCDMA_OAM_H__
#define __WG_WCDMA_OAM_H__

void wcdma_oam_power_set(wcdma_state_t *wcdma_state, uint16_t power);
void wcdma_oam_reset(wcdma_state_t *wcdma_state);
void wcdma_oam_rf_set(wcdma_state_t *wcdma_state, uint8_t flag);
void wcdma_oam_lbs_set(wcdma_state_t *wcdma_state, uint8_t flag);
void wcdma_oam_para_set(wcdma_state_t *wcdma_state);

#endif 