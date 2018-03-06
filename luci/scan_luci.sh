#!/bin/sh

cp scan /etc/config

cp admin/scan.lua /usr/lib/lua/luci/controller/admin

mkdir /usr/lib/lua/luci/model/cbi/admin_scan
cp model/scan.lua /usr/lib/lua/luci/model/cbi/admin_scan

mkdir /usr/lib/lua/luci/view/admin_scan
cp  scan/scan.htm  /usr/lib/lua/luci/view/admin_scan

cp usrupgrade /usr/bin


