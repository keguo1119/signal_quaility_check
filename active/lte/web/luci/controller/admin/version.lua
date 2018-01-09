
module("luci.controller.admin.version", package.seeall)

function index()
	local fs = require "nixio.fs"
	local page

	page = node("admin", "version")
	page.target = firstchild()
	page.title = _("Version")
	page.order = 120
	page.index = true	

--	entry({"admin", "version"}, alias("admin", "version", "flashops"), _("Version "), 120)
	page = entry({"admin", "version", "flashops"}, call("action_flashops"), _("Version control"), 1)
	page = entry({"admin", "version", "status"}, template("admin_version/status"), _("Status"), 2)
	page = entry({"admin", "version", "lteip_change"}, cbi("admin_version/lteip"), nil)
--	page = entry({"admin", "version", "lteip_change"}, call("ip_change"), nil)
	page.leaf = true
--	page = entry({"admin", "version", "lteip"}, cbi("admin_version/lteip"))
--	page = entry({"admin", "version", "lteip"}, cbi("admin_version/lteip"), _("LTE IP"), 3)
	
end



function action_flashops()
	local sys = require "luci.sys"
	local fs  = require "nixio.fs"

	local upgrade_avail = fs.access("/lib/upgrade/platform.sh")
	local reset_avail   = os.execute([[grep '"rootfs_data"' /proc/mtd >/dev/null 2>&1]]) == 0

        local backup_cmd  = "sysupgrade1 --create-backup - 2>/dev/null"
--      local backup_cmd  = "verupgrade -b - 2>/dev/null"
	local restore_cmd = "tar -xzC/usr/local/lte >/dev/null 2>&1"
	local app_cmd = "tar -xzC/usr/local/lte >/dev/null 2>&1"
	local app_tmp = "/tmp/app.tar.gz"	
	local luci_tmp = "/tmp/luci.tar.gz"

	local function app_supported()
		return (os.execute("sysupgrade1 -l %q >/dev/null" % app_tmp) == 0)
	end

	local function luci_supported()
		return (os.execute("sysupgrade1 -w %q >/dev/null" % luci_tmp) == 0)
	end

	local function image_supported()
		return (os.execute("sysupgrade -T %q >/dev/null" % image_tmp) == 0)
	end

	local function image_checksum()
		return (luci.sys.exec("md5sum %q" % image_tmp):match("^([^%s]+)"))
	end

	local function storage_size()
		local size = 0
		if fs.access("/proc/mtd") then
			for l in io.lines("/proc/mtd") do
				local d, s, e, n = l:match('^([^%s]+)%s+([^%s]+)%s+([^%s]+)%s+"([^%s]+)"')
				if n == "linux" or n == "firmware" then
					size = tonumber(s, 16)
					break
				end
			end
		elseif fs.access("/proc/partitions") then
			for l in io.lines("/proc/partitions") do
				local x, y, b, n = l:match('^%s*(%d+)%s+(%d+)%s+([^%s]+)%s+([^%s]+)')
				if b and n and not n:match('[0-9]') then
					size = tonumber(b) * 1024
					break
				end
			end
		end
		return size
	end


	local fp
	luci.http.setfilehandler(
		function(meta, chunk, eof)
			if not fp then
				if meta and meta.name == "image" then
					fp = io.open(app_tmp, "w")
				else
					fp = io.open(luci_tmp, "w")
				end
			end
			if chunk then
				fp:write(chunk)
			end
			if eof then
				fp:close()
			end
		end
	)

	if luci.http.formvalue("backup") then
		--
		-- Assemble file list, generate backup
		--
		local reader = ltn12_popen(backup_cmd)
		luci.http.header('Content-Disposition', 'attachment; filename="backup-%s-%s.tar.gz"' % {
			luci.sys.hostname(), os.date("%Y-%m-%d")})
		luci.http.prepare_content("application/x-targz")
		luci.ltn12.pump.all(reader, luci.http.write)

	elseif luci.http.formvalue("restore") then
		--
		-- Unpack received .tar.gz
		--
		local step = tonumber(luci.http.formvalue("step") or 1 )                              
        	if step ==1  then                                                                    
                                                                                                    
	        	if luci_supported() then                                                      
        	            fs.unlink(luci_tmp)                                                   
                        luci.template.render("admin_version/applyreboot")                    
                        luci.sys.myreboot()                                                    
                 else
					 	fs.unlink(luci_tmp) 
						luci.template.render("admin_version/flashops", {                   
									 reset_avail   = reset_avail,                                 
                                     upgrade_avail = upgrade_avail,                               
                                     luci_invalid = true                                         
	               		})                                                                   
				end
             end                            

	elseif luci.http.formvalue("image") then
		--
		--  upload app
		--
               local step = tonumber(luci.http.formvalue("step") or 1 )
               if step ==1  then
	
					if app_supported() then
					      fs.unlink(app_tmp)
						  luci.template.render("admin_version/applyreboot")
						  luci.sys.myreboot() 
					else
					      fs.unlink(app_tmp)
						  luci.template.render("admin_version/flashops", {
                                    reset_avail   = reset_avail,
                                    upgrade_avail = upgrade_avail,
                                    image_invalid = true
                                })
					end
			   end

	elseif reset_avail and luci.http.formvalue("reset") then
		--
		-- Reset system
		--
		luci.template.render("admin_version/applyreboot", {
			title = luci.i18n.translate("Erasing..."),
			msg   = luci.i18n.translate("The system is erasing the configuration partition now and will reboot itself when finished."),
			addr  = "192.168.1.1"
		})
		fork_exec("killall dropbear uhttpd; sleep 1; mtd -r erase rootfs_data")
	else
		--
		-- Overview
		--
		luci.template.render("admin_version/flashops", {
			reset_avail   = reset_avail,
			upgrade_avail = upgrade_avail
		})
	end
end

function action_reboot()
        local reboot = luci.http.formvalue("reboot")
        luci.template.render("admin_system/reboot", {reboot=reboot})
        if reboot then
                luci.sys.reboot()
        end
end

function fork_exec(command)
	local pid = nixio.fork()
	if pid > 0 then
		return
	elseif pid == 0 then
		-- change to root dir
		nixio.chdir("/")

		-- patch stdin, out, err to /dev/null
		local null = nixio.open("/dev/null", "w+")
		if null then
			nixio.dup(null, nixio.stderr)
			nixio.dup(null, nixio.stdout)
			nixio.dup(null, nixio.stdin)
			if null:fileno() > 2 then
				null:close()
			end
		end

		-- replace with target command
		nixio.exec("/bin/sh", "-c", command)
	end
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

function ip_change()
	local cbi = require "luci.cbi"
	local tpl = require "luci.template"
--	local map = luci.cbi.load("admin_version/system")[1]
	local map = luci.cbi.load("admin_version/admin")[1]
--	local map = luci.cbi.load("admin_version/network")[1]
--	tpl.render("header")
	map:render()
--	tpl.render("footer")
--	luci.template.render("admin_version/flashops")
end


