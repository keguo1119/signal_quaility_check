#ifndef __MAIN_H__
#define __MAIN_H__

#include "oss.h"
#include "config.h"
#include "protocol_common.h"
#include "modem.h"
#include "scan_modem.h"
#include "scan_file.h"
#include "scan_cfg.h"
//#include "scan_net.h"
#include <stdio.h>
#include <stdlib.h>

#define SCAN_DEBUG 1

#define SYS_STAT_ON_LINE         1
#define SYS_STAT_ONLOAD          2
#define SYS_STAT_TAKEOFF         3
#define SYS_STAT_GO_UP           4
#define SYS_STAT_P2P             5
#define SYS_STAT_GO_DOWN         6
#define SYS_STAT_DELIVER         7
#define SYS_STAT_GOBACK_UP       8
#define SYS_STAT_GOBACK_P2P      9
#define SYS_STAT_GOBACK_DOWN    10
#define SYS_STAT_LAND           11
#define SYS_STAT_MISSION_OK     18
#define SYS_STAT_FLY_WAIT       20
#define SYS_STAT_PAUSE          21
#define SYS_STAT_GS_CTRL        22
#define SYS_STAT_EMER_ROUTE     30
#define SYS_STAT_EMER_DOWN      31
#define SYS_STAT_EMER_LAND      32
#define SYS_STAT_MANUAL_CONTROL 33
#define SYS_STAT_REMOTE_CONTROL 34
#define SYS_STAT_FC_DISCONNECT  35
#define SYS_STAT_EMER_LOAD      36
#define SYS_STAT_WAIT_FLY       52
#define SYS_STAT_IMAGE_IDENTIFY 55
#define SYS_STAT_IMAGE_ALIGN    56  // ¶Ô×¼×´Ì¬
#define SYS_STAT_LANDING        65

#endif

