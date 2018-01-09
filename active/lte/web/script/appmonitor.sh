#!/bin/sh

#Program:
#	This program is to start some other processes when the linux turn on 
#	and monitor the processes. When the processes terminate abnormally, this#	program will restart them

process_reboot(){
	ps  | grep lteapp/./app | grep -v grep |awk '{print $1}' | xargs kill -9
	
	PROCESS_NUM=`ps | grep lteapp/./app | grep -v "grep" | wc -l` 
	echo "PROCESS_NUM1 = "$PROCESS_state""

	if [ $PROCESS_NUM -eq 0 ]; then
		/usr/local/lte/workspace/lteapp/./app &
		echo "lteapp restart"
	else
		echo "reboot the system"
		ifconfig eth0 down	
		ifconfig apcli0 down
		wifi down
		reboot
	fi 
}



processes_check() {
	PROCESS_NUM=`ps | grep lteapp/./app | grep -v "grep" | wc -l` 
	echo "PROCESS_NUM1 = "$PROCESS_state""

	if [ $PROCESS_NUM -eq 0 ]; then
		sleep 3
		/usr/local/lte/workspace/lteapp/./app &
		echo "lteapp restart"
	elif [ $PROCESS_NUM -eq 1 ]; then
		PROCESS_state=`ps | grep lteapp/./app | grep -v "grep" | awk '{print $4}'`
		echo "lteapp state is "$PROCESS_state""
		if [  "$PROCESS_state" = "R"  ] ; then 
			echo "lteapp is running"
		elif  [  "$PROCESS_state" = "S" ]; then
			echo "lteapp is sleeping"
		else
			process_reboot
		fi
	else 
		process_reboot
	fi
}

for (( i=1; i<6; i=i+1 ))
do 

	processes_check
	echo "appminitor operated "
	sleep 10
done
exit 0

