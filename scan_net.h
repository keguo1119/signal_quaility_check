#ifndef __SCAN_NET_H__
#define __SCAN_NET_H__

#include <inttypes.h>

typedef struct
{
	uint8_t msg_id;
	uint32_t id_uav_xyi;
	uint32_t time_boot_ms;
	uint8_t fix_type;
	int32_t lat_gps;
	int32_t lon_gps;
	int32_t alt_rel_pres;
	uint16_t eph;
	uint16_t epv;
	uint16_t vel_gps;
	uint16_t cog;
	uint8_t satellites_visible;
}__attribute__ ((packed)) gps_raw_t;

int scan_net_init();
void scan_net_run();
int scan_net_gps_info_get(float *lat, float *lon);
#endif