-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.


module("luci.controller.admin.scan", package.seeall)

function index()
        entry({"admin", "scan"}, alias("admin", "scan", "config"), _("XunYi"), 90).index = true
        entry({"admin", "scan", "config"}, cbi("admin_scan/scan"), _("Config"), 1)
        entry({"admin", "scan", "debug"}, call("action_debug"), _("Debug"), 2)
end

function action_debug()
--        local dmesg = luci.sys.dmesg()
        local dmesg = luci.util.exec("cat /root/scanner/err_info.txt")
        luci.template.render("admin_scan/dmesg", {dmesg=dmesg})
end
