#!/bin/sh

date_t=$(date +"%Y-%m-%d-%H-%M-%S")

for i in $(seq 0 2)
do
        ipaddr=$(ifconfig usb$i | grep "inet addr" | awk '{print $2}' | awk 'BEGIN {FS=":"} {print$2}')
        echo $ipaddr
        if [ "$ipaddr" = "" ]; then
                continue
        fi

        date +"%Y-%m-%d %H:%M:%S" > /root/scanner/iperf/iperf_usb"$i"_info-"$date_t".txt
        iperf -u -c 120.27.136.251 -i 1 -b 2M -t 86400 -B $ipaddr >> /root/scanner/iperf/iperf_usb"$i"_info-"$date_t".txt  2>&1 &
done

# kill $(ps | grep iperf | grep -v grep | awk '{print $1}')