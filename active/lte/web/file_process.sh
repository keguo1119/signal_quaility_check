#!/bin/sh

mkdir -p /usr/local/lte/workspace/lteapp
echo " mkdir -p /usr/local/lte/workspace/lteapp"

ln -s /usr/local/lte/workspace/ /usr/local/lte/back1
echo "ln -s //usr/local/lte/workspace/ /usr/local/lte/back1"


cp -r luci/   /usr/lib/lua/
echo "copy luci file to  /usr/lib/lua/"

cp -r luci/   /usr/local/lte/workspace/
echo "copy luci file to  /usr/local/lte/workspace"

mv config_file/version /usr/local/lte/workspace/
echo "mv version /usr/local/lte/workspace/"

mv config_file/app /usr/local/lte/workspace/lteapp
echo "mv app /usr/local/lte/workspace/lteapp"


cp  config_file/config/lteiptest /etc/config/
echo "cp lteiptest /etc/config/"

mv config_file/version /usr/local/lte/workspace/
echo "mv version /usr/local/lte/workspace/"

cp config_file/verupgrade.conf /lib/upgrade/
echo "cp  cp config_file/verupgrade.conf /lib/upgrade/"


mv config_file/rc.local /etc/
echo "change the rc.local"

mv config_file/crontabs/root  /etc/crontabs/
echo "change the crontab"

cp   script/sysupgrade1 /sbin/
echo "mv script/sysupgrade1 /sbin/"

cp   script/appmonitor.sh script/pingtest.sh  /usr/local/lte/workspace/luci/
mv "appmonitor.sh script/pingtest.sh /usr/local/lte/workspace/luci/"

cd system/
mv libpthread.so.0 librt.so.0 /usr/lib/
echo "libpthread.so.0 librt.so.0 /usr/lib/"

opkg install  zlib_1.2.8-1_ramips_24kec.ipk  libmagic_5.11-1_ramips_24kec.ipk file_5.11-1_ramips_24kec.ipk 

opkg install libncurses_5.9-1_ramips_24kec.ipk   minicom_2.7-1_ramips_24kec.ipk

cd ..
cp -r config_file/ script/  /usr/local/lte/workspace/luci/

rm -r /tmp/luci-indexcache  /tmp/luci-modulecache/
