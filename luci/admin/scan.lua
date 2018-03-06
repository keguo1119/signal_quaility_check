-- Copyright 2008 Steven Barth <steven@midlink.org>
-- Copyright 2011 Jo-Philipp Wich <jow@openwrt.org>
-- Licensed to the public under the Apache License 2.0.


module("luci.controller.admin.scan", package.seeall)

function index()
        local fs = require "nixio.fs"
        entry({"admin", "scan"}, alias("admin", "scan", "config"), _("XunYi"), 90).index = true
        entry({"admin", "scan", "config"}, cbi("admin_scan/scan"), _("Config"), 1)
        entry({"admin", "scan", "debug"}, call("action_debug"), _("Debug"), 2)
        entry({"admin", "scan", "download"}, call("action_flashops"), _("Download"), h)
        entry({"admin", "scan", "download", "action"}, post("action_down"))   
end

function action_debug()
        os.execute("cat $(ls ~/scanner/file/4G_signal_scan*  -tl | awk '{print $9}' | head -n 1) | head -n 2  > ~/scanner/file/signal_info.txt")
        os.execute("cat $(ls ~/scanner/file/4G_signal_scan*  -tl | awk '{print $9}' | head -n 1) | tail   >> ~/scanner/file/signal_info.txt")
        os.execute("cat /root/scanner/err_info.txt > /tmp/scanner_log_err_info.txt")
        os.execute("cat $(ls /root/scanner/log/log-* -tl | awk '{print $9}' | head -n 1) | tail -n 5 >> /tmp/scanner_log_err_info.txt")

        local dmesg     = luci.util.exec("cat /tmp/scanner_log_err_info.txt")
        local scan_info = luci.util.exec("cat /root/scanner/file/signal_info.txt")
        luci.template.render("admin_scan/dmesg", {dmesg=dmesg, scan_info=scan_info})
end

local function supports_sysupgrade()                                                                          
        return nixio.fs.access("/lib/upgrade/platform.sh")                                   
end                                                                                                  
                                                                                             
local function supports_reset()                                                                                                                     
        return (os.execute([[grep -sqE '"rootfs_data"|"ubi"' /proc/mtd]]) == 0)                      
end  

function action_flashops()                                                                               
        --                                                                                           
        -- Overview                                                                                           
        --                                                                                                    
        luci.template.render("admin_scan/download", {                 
                reset_avail   = supports_reset(),                       
                upgrade_avail = supports_sysupgrade()                            
        })                                                                                                                                          
end  

function action_down()                                                                                                                            
        local reader = ltn12_popen("usrupgrade --create-backup - 2>/dev/null")                                                                      
        luci.http.header(                                                                                                                           
                'Content-Disposition', 'attachment; filename="4G_info-%s.tar.gz"' %{                                                              
                        os.date("%Y-%m-%d")                                                                                                         
                })                                                                                                                                  
                                                                                                         
        luci.http.prepare_content("application/x-targz")                                                 
        luci.ltn12.pump.all(reader, luci.http.write)                                                     
end  

function ltn12_popen(command)                                                                                                                
                                                                                                                                             
        local fdi, fdo = nixio.pipe()                                                                                                        
        local pid = nixio.fork()                                                                                                             
                                                                                                                                             
        if pid > 0 then                                                                                                                      
                fdo:close()                                                                                                                  
                local close                                                                                                                  
                return function()                                                                                                            
                        local buffer = fdi:read(2048)                                                                                        
                        local wpid, stat = nixio.waitpid(pid, "nohang")                                                                      
                        if not close and wpid and stat == "exited" then                                                                      
                                close = true                                                                                                 
                        end                                                                                                                  
                                                                                                                                             
                        if buffer and #buffer > 0 then                                                                                       
                                return buffer                                                                                                
                        elseif close then                                                                                                    
                                fdi:close()                                                                                                  
                                return nil                                                                                                   
                        end                                                                                                                  
                end                                                                                                                          
        elseif pid == 0 then                                                                                                                 
                nixio.dup(fdo, nixio.stdout)                                                                                                 
                fdi:close()                                                                                                                  
                fdo:close()                                                                                                                  
                nixio.exec("/bin/sh", "-c", command)                                                                                         
        end                                                                                                                                  
end