#include "scan_gps.h"
#include "oss.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static float lon = 39.48327, lat = 129.43280;
    pthread_t gps_id;

void scan_gps_info_get(float *lon_t, float *lat_t)
{
    *lon_t = lon;
    *lat_t = lat;
}

void *scan_gps_run_pthread()
{
    float lon_t, lat_t;

    while(1) {
        scan_gps_info_get(&lon_t, &lat_t);
 //       printf("lon=%f, lat=%f\n", lon_t, lat_t);
        oss_delay(500);
    }

}

void scan_gps_run()
{
    if(pthread_create(&gps_id, NULL, (void *)scan_gps_run_pthread, NULL)) {
        printf("%s: created pthread failed!\n", __func__);
    } 
}
