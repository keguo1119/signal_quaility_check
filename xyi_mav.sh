#!/bin/bash

# for i in $@; do
	# echo $i
# done

prefix="[ "`basename $0`" ]"

# 确保此脚本为互斥运行
lock="/var/lock/xyi_mav.lock"
if [ -s $lock ];then	# 文件存在且非空
	pid_record=`cat ${lock}`
	# echo $pid_record
	if_true_lock=`ps aux | grep "$pid_record" | grep "xyi_mav.sh"`
	if [ -n "$if_true_lock" ];then
		echo "${lock} is locked by pid" `cat ${lock}`
		exit 0
	else
		rm $lock
	fi
fi	

echo $$ > $lock 	# 将自身PID写入.lock文件
echo "${prefix} xyi_mav script is running..."

# 获取安装路径信息
install_info="/etc/xyi/install.info"
if [ -f $install_info ];then
	xyi_installation_path=`cat ${install_info} | grep "INSTALLATION_PATH" | awk -F '=' '{print $2}'`
	if [ -n "$xyi_installation_path" ];then
		echo "${prefix} installation path: ${xyi_installation_path}"
	else
		echo "${prefix} INSTALLATION_PATH not found"
		exit 0
	fi
else
	echo "${prefix} ${install_info} not found"	
	exit 0
fi

# /home -> /home/
# /home/ -> /home/
function to_path()
{
	if [ "${1: -1}" != "/" ];then
		echo "${1}/"
	else
		echo ${1}
	fi
}

xyi_installation_path=$(to_path $xyi_installation_path)

# 生成相应目录
log_path="${xyi_installation_path}log/"
cache_path="${xyi_installation_path}cache/"
tmp_path="${xyi_installation_path}tmp/"
config_path="${xyi_installation_path}config/"

install -d $log_path				# 确认创建相应目录
install -d $cache_path
install -d $tmp_path
install -d $config_path

tmp_files="${tmp_path}*"
if_empty=`ls $tmp_path | wc -l`
if [[ $if_empty -ne 0 && "$tmp_files" != "/" && -z "`echo $tmp_files | grep "//"`" ]];then
	echo "${prefix} clear ${tmp_files}"
	rm -r $tmp_files
fi

# 将启动信息(时间等)写入${log_path}startup/startup.log
startup_log_path="${log_path}startup/"
install -d $startup_log_path
startup_log="${startup_log_path}startup.log"
startup_time=`date`
echo "${prefix} Startup at ${startup_time}" >> ${startup_log}


startup_cache_path="${cache_path}startup/"
install -d $startup_cache_path
file_to_count="${startup_cache_path}power_on.count"
if [ ! -s $file_to_count ];then
	touch $file_to_count
	count=0
else
	count=`cat $file_to_count`
	count=`awk -v count=$count 'BEGIN {printf "%d", count+1}'`
fi
echo $count > $file_to_count
echo "${prefix} new power-on count is ${count}"

time=`date +%Y%m%d%H%M%S`

count_time="${count}-${time}"

log_directory="${log_path}${count_time}/"
install -d $log_directory	
echo "${prefix} log: ${log_directory}"


# 启动各个模块
bin_path="${xyi_installation_path}bin/"

shutdown_path="${bin_path}shutdown/"
shutdown_elf="${shutdown_path}shutdown"
shutdown_log="${log_directory}shutdown.log"
shutdown_out="${log_directory}shutdown.out"
echo "${prefix} ${shutdown_elf}"
chmod 755 $shutdown_elf
path_backup=`pwd`
cd $shutdown_path
$shutdown_elf $shutdown_log > $shutdown_out 2>&1 &
cd $path_backup

auto_copy_path="${bin_path}auto_copy/"
auto_copy_script="${auto_copy_path}auto-copy"
auto_copy_out="${log_directory}auto_copy.out"
echo "${prefix} ${auto_copy_script}"
chmod 755 $auto_copy_script
sed -i 's/\r//' $auto_copy_script										# 去除\r
$auto_copy_script > $auto_copy_out 2>&1 &

cache_cleanup_path="${bin_path}startup/"
cache_cleanup_script="${cache_cleanup_path}cache_cleanup.sh"
cache_cleanup_out="${log_directory}cache_cleanup.out"
echo "${prefix} ${cache_cleanup_script}"
chmod 755 $cache_cleanup_script
sed -i 's/\r//' $cache_cleanup_script	
path_backup=`pwd`
cd $cache_cleanup_path
$cache_cleanup_script > $cache_cleanup_out 2>&1
cache_cleanup_script_ret=$?
cd $path_backup

network_setup_path="${bin_path}startup/"
network_setup_script="${network_setup_path}network_setup.sh"
network_setup_out="${log_directory}network_setup.out"
echo "${prefix} ${network_setup_script}"
chmod 755 $network_setup_script
sed -i 's/\r//' $network_setup_script	
path_backup=`pwd`
cd $network_setup_path
$network_setup_script > $network_setup_out 2>&1
network_setup_script_ret=$?
cd $path_backup


if [ $network_setup_script_ret -ne 0 ];then
	echo "${prefix} failed to setup network"
	exit 1
fi

log_of_ifstat="${log_directory}ifstat.log"
{
while true
do	
	echo "" >> $log_of_ifstat
	date +"%Y-%m-%d %H:%M:%S" >> $log_of_ifstat
	# 'apt-get install ifstat' to install ifstat
	ifstat -tna >> $log_of_ifstat 2>&1
	sleep 2
done
}&

log_of_iperf="${log_directory}iperf.log"
{
while true
do	
	echo "" >> $log_of_iperf
	date +"%Y-%m-%d %H:%M:%S" >> $log_of_iperf
	# 'apt-get install iperf' to install iperf
	iperf -u -c 120.27.136.251 -i 1 -b 2M -t 86400 >> $log_of_iperf 2>&1
	sleep 2
done
}&

log_of_ping="${log_directory}ping.log"
{
while true
do	
	echo "" >> $log_of_ping
	date +"%Y-%m-%d %H:%M:%S" >> $log_of_ping
	ping www.baidu.com >> $log_of_ping 2>&1
	sleep 2
done
}&

function Terminate_Process(){
	pid=`ps aux | grep $1 | grep -v grep | sed -n 1p | awk '{print $2}'`
	if [ -n "$pid" ];then	# 非空
		expr $pid + 0 &>/dev/null
		if [ $? -eq 0 ];then		# 是整数
			kill -9 $pid
		fi
	fi
}

while true;do
	Terminate_Process "uvcdynctrl"
	v4l2_log="/var/log/uvcdynctrl-udev.log"
	if [ -f $v4l2_log ];then 
		rm $v4l2_log
	fi
	sleep 20
done