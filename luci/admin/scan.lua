---- Copyright 2008 Steven Barth <steven@midlink.org>^M
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>^M
-- Licensed to the public under the Apache License 2.0.^M

module("luci.controller.admin.scan", package.seeall)

function index()
        entry({"admin", "scan"}, alias("admin", "scan", "config"), _("XunYi"), 90).index = true
        entry({"admin", "scan", "config"}, cbi("admin_scan/scan"), _("Config"), 1)
        entry({"admin", "scan", "debug"}, call("action_debug"), _("Debug"), 2)
end

function action_debug()
        os.execute("cat $(ls ~/scanner/file/4G_signal_scan*  -tl | awk '{print $9}' | head -n 1) | head -n 2  > ~/scanner/file/signal_info.txt")
        os.execute("cat $(ls ~/scanner/file/4G_signal_scan*  -tl | awk '{print $9}' | head -n 1) | tail   >> ~/scanner/file/signal_info.txt")

        local dmesg     = luci.util.exec("cat /root/scanner/err_info.txt")
        local scan_info = luci.util.exec("cat /root/scanner/file/signal_info.txt")
        luci.template.render("admin_scan/scan", {dmesg=dmesg, scan_info=scan_info})
end
