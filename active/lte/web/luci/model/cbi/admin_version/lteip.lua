-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.

local sys   = require "luci.sys"
local zones = require "luci.sys.zoneinfo"
local fs    = require "nixio.fs"
local conf  = require "luci.config"

local m, s, o
m = Map("lteiptest", translate("LTE IP set"), translate("Here you can input the ip address of  your lte board and then check connection status  of the WRT to LTE ."))
m:chain("luci")


s = m:section(TypedSection, "lteip", translate(""))
s.anonymous = true
s.addremove = false


o = s:option( Value, "lteip", translate("Input LTE board ip: "))
o.optional    = true
o.placeholder = "0.0.0.0"
o.datatype = "ip4addr"

--s:tab("general",  translate("General Settings"))

--[[
--o = s:option( Value, "lteip", translate("Hostname"))
o = s:taboption("general", Value, "lteip", translate("Input LTE board ip: "))
o.optional    = true
o.placeholder = "0.0.0.0"
o.datatype = "ip4addr"
--]]

return m

