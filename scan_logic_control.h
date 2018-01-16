#ifndef __SCAN_LOGIC_CONTROL_H__
#define __SCAN_LOGIC_CONTROL_H__

void scan_logic_control_run();
void scan_logic_oper_mode_set(int mode);
void scan_logic_gps_get(float *lat, float *lon);
#endif