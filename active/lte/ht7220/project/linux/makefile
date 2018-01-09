objs = App.o dlist.o Modem.o Plmn.o Uart.o MobileRoute.o ModemCtrl.o log.o NetworkConnect.o SmsPduCode.o config.o oss.o
src=$(wildcard *.c)
inc = log.h NetworkConnect.h SmsPduCode.h dlist.h MobileDetectCommon.h ModemCommon.h Plmn.h Uart.h MobileRoute.h
CC = gcc


silentdetect: $(objs)
	$(CC) $(objs) -lpthread -o silentdetect  

%.o:%.c
	$(CC) -g -o $@  -c $< 

.PHONY:clean
clean:
	rm $(objs)
	rm silentdetect 	

