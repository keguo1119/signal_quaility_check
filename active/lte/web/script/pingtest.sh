#!/bin/sh

local PingFlag

sleep 25

sed -i  "s/'//g" /etc/config/lteip

myreboot() {
	ifconfig eth0 down
	ifconfig apcli0 down
	kill $(ps | grep app | grep -v grep | awk '{print $1}') 
	reboot

}

pingtestwrtlte() {
    local ip
    read ip < /etc/config/lteip
    echo $ip
    ping -c2 $ip 
    if [ $? -ne 0 ]; then
	echo "reboot"
        date >> testmessage
        echo "the net is not conneted" >> testmessage
	myreboot
	return 1
    fi 
	echo "The net is connected"
	return 0
}

read PingFlag <  PingStatus.txt

if [ $PingFlag -eq 0 ]; then 
	echo 1 > PingStatus.txt
	pingtestwrtlte
	if [ $? -eq 0 ]; then
		echo 0 > PingStatus.txt
	fi 
elif [ $PingFlag -eq 1 ]; then
	date >> testmessage
	echo "the first reboot also can't work!" >> testmessage
	echo 2 > PingStatus.txt
	pingtestwrtlte
	if [ $? -eq 0 ]; then  
                echo 0 > PingStatus.txt
        fi          
elif [ $PingFlag -eq 2 ]; then
	date >> PingStatus.txt
	echo "the second reboot also can't work!" >> testmessage
	echo 0 > PingStatus.txt
	sleep 100
	reboot
fi
