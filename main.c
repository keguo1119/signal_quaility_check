#include "main.h"
#include "scan_cfg.h"
#include "oss.h"
#include "config.h"
#include "protocol_common.h"
#include "modem.h"
#include "scan_modem.h"
#include <stdio.h>
#include <stdlib.h>
#include "scan_gps.h"
#include "scan_logic_control.h"

char *sys_cfg_path = "./";
char *sys_cfg_file = "sys_cfg";
char *usr_cfg_path = "./";
char *usr_cfg_file = "usr_cfg";

void scanner_init()
{
    scan_cfg_init(NULL, NULL);
    scan_modem_init();
    scan_net_init();
}

void scanner_run()
{
    scan_modem_run();
    scan_gps_run();
    scan_net_run();
    scan_logic_control_run();   
}

int main(int argc, char ** argv)
{
    scanner_init();

    scanner_run();

    while(1);
    printf("run OK!\n");
}