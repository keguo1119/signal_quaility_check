#ifndef __WG_GSM_H__
#define __WG_GSM_H__


/////////////////////////////////////////////////////////////////////
void gsm_run();
int  gsm_init();

int gsm_cfg_init(gsm_state_t *gsm_state, uint8_t channel, uint8_t trx, 
				 uint32_t peer_ip_addr, uint16_t peer_port, uint32_t uart_ip_addr);

#endif 