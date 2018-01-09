-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Licensed to the public under the Apache License 2.0.



local io     = require "io"
local os     = require "os"
local table  = require "table"
local nixio  = require "nixio"
local fs     = require "nixio.fs"
local uci    = require "luci.model.uci"

local luci  = {}
luci.util   = require "luci.util"
luci.ip     = require "luci.ip"

local tonumber, ipairs, pairs, pcall, type, next, setmetatable, require, select =
        tonumber, ipairs, pairs, pcall, type, next, setmetatable, require, select


module "luci.sys"


function lteversion(num, newname)
        filename = "/usr/local/lte/back"..num.."/version"
        local file = io.open(filename, "r")
        if file then
                if newname == 1 then
                         local name = file:read("*line")
                         file:close()
                         return name
                else
                        local name = file:read("*line")
                        name = file:read("*line")
                         file:close()
                         return name
                end
        else
                return nil
        end
end

function myreboot()
	os.execute("ifconfig eth0 down ");  --shutdown the eth
	os.execute("ifconfig apcli0 down"); --shutdown the WIFI
	os.execute(" kill $(ps | grep app | grep -v grep | awk '{print $1}') ")
	os.execute("reboot > /dev/null 2>&1");
end

function lteip()
	lteipfile = "/etc/config/lteip"         
        local file = io.open(lteipfile, "r"); 
	 local lteip = file:read("*line") 
	 file:close() 
	return lteip
end 

function pingtestwrtlte()
	lteipfile = "/etc/config/lteip"
	os.execute("cat /etc/config/lteiptest | sed -n 3p | awk   '{print $3}'  >/etc/config/lteip")
	local file = io.open(lteipfile, "r");
	if file then 
		local lteip = file:read("*line")
		file:close()
		cmd = "ping -c1 "..lteip.." >/dev/null 2>&1 "
		return os.execute(cmd)
--		return os.execute("ping -c1 192.168.8.246 >/dev/null 2>&1")
	else
		return os.execute("ping -c1 192.168.8.162 >/dev/null 2>&1")
	end
end

function lteipwrite(newname)
    if type(newname) == "string" and #newname > 0 then
        fs.writefile( "/etc/config/lteip", newname )
        return newname
    else
        return nixio.uname().nodename
    end 
end

