#!/bin/sh

cp scan /etc/config

cp amdin/scan.lua /usr/lib/lua/luci/controller/admin

mkdir /usr/lib/lua/luci/model/cbi/admin_scan
cp model/scan.lua /usr/lib/lua/luci/model/cbi/admin_scan

mkdir /usr/lib/lua/luci/view/admin_scan
cp  /usr/lib/lua/luci/view/admin_status/dmesg.htm  /usr/lib/lua/luci/view/admin_scan


