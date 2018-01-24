#ifndef __SCAN_LOGIC_CONTROL_H__
#define __SCAN_LOGIC_CONTROL_H__

enum LogicCtrlMainStateID
{
    LC_MS_NORMAL = 101,
    LC_MS_GOBACK = 102,
    LC_MS_EMER   = 103,
};

enum LogicCtrlSubStateID
{
    LC_SS_ONLINE = 1,
    LC_SS_ONLOAD = 2,
    LC_SS_WAITFLY = 52,
    LC_SS_TAKEOFF = 3,
    LC_SS_CLIMB = 4,
    LC_SS_CRUISING = 5,
    LC_SS_IMAGE_IDENTIFY = 55,
    LC_SS_IMAGE_ALIGN = 56,
    LC_SS_DESCEND = 6,
    LC_SS_IMAGE_LANDING = 65,
    LC_SS_ADJUST_YAW = 7,
    LC_SS_LANDED_NORMAL = 11,
    LC_SS_LANDED_REMOTE_STOP = 12,
    LC_SS_LANDED_AUTO_STOP = 13,
    LC_SS_LANDED_GS_CTRL = 14,
    LC_SS_PAUSE = 21,
    LC_SS_GROUND_CTRL = 22,
    LC_SS_APPROACH_CTRL = 23,
    LC_SS_APPROACH_CTRL_PAUSE = 24,
    LC_SS_CLOUD_CTRL = 33,
    LC_SS_MANUAL    = 34,
    LC_SS_LOSE_CTRL = 35,
    LC_SS_PAUSE_EMER_LOAD = 36,
};

void scan_logic_control_run();
void scan_logic_oper_mode_set(int mode);
void scan_logic_gps_get(float *lat, float *lon);
void scan_logic_uav_status_set(unsigned char status);
unsigned char scan_logic_uav_status_get(void );
void scan_logic_main_status_set(unsigned char status);
unsigned char scan_logic_main_status_get(void );
void scan_logic_time_std_set(unsigned int time_std);
unsigned int scan_logic_time_std_get(void );

#endif