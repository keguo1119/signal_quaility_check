#!/bin/sh

used_disk_precent=`df -h | grep /dev/mtdblock6 | awk '{print $5}' | grep -o '[0-9]\+'`

if [ $used_disk_precent -ge 50 ]; then
    cd /root/scanner/log/
    rm $(ls -tl | awk '{print $9}'  | tail -n 5)
    cd /root/scanner/iperf/
    rm $(ls -tl | awk '{print $9}'  | tail -n 20)

    cd /root/scanner/file/
    rm $(ls -tl | awk '{print $9}'  | tail -n 5)
fi