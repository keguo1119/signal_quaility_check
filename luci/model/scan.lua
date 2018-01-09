-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"

local m, s, o
local has_ntpd = fs.access("/usr/sbin/ntpd")

m = Map("scan", translate("Config"), translate("Here you can chose how the signal scanner equitment works ."
m:chain("luci")

s = m:section(TypedSection, "mode", translate("System Properties"))
s.anonymous = true
s.addremove = false

o = s:option(Flag, "scan_oper_auto", translate("The Device auto to change opertion mode"))
o.enabled  = "1"
o.disabled = "0"

o = s:option(ListValue, "scan_oper_mode", translate("Chose the operation mode"))
o:value(0, "4G")
o:value(1, "3G")

return m
