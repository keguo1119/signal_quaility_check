#ifndef __LTE_HT7220_H__
#define __LTE_HT7220_H__


/////////////////////////////////////////////////////////////////////
void lte_run();
int  lte_init();

int lte_cfg_init(lte_state_t *lte_state, uint8_t dst_bsId, uint8_t dst_modId, uint8_t src_bsId, 
                 uint8_t src_modId, uint32_t peer_ip_addr, uint16_t peer_port);
#endif 