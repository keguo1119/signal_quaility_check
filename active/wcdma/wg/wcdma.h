#ifndef __WCDMA_HT7220_H__
#define __WCDMA_HT7220_H__


/////////////////////////////////////////////////////////////////////
void wcdma_run();
int  wcdma_init();

int wcdma_cfg_init(wcdma_state_t *wcdma_state, uint8_t channel, uint8_t trx, uint32_t peer_ip_addr, uint16_t peer_port);

#endif 