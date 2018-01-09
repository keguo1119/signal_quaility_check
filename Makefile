OSS_PATH  = ./util

oss_objs = $(OSS_PATH)/dlist.o $(OSS_PATH)/oss_uart.o  $(OSS_PATH)/oss_log.o $(OSS_PATH)/oss_net.o $(OSS_PATH)/config.o $(OSS_PATH)/oss.o $(OSS_PATH)/oss_time.o $(OSS_PATH)/oss_gpio.o
oss_src = $(OSS_PATH)/dlist.c $(OSS_PATH)/oss_uart.c  $(OSS_PATH)/oss_log.c $(OSS_PATH)/oss_net.c $(OSS_PATH)/config.c $(OSS_PATH)/oss.c $(OSS_PATH)/oss_time.c $(OSS_PATH)/oss_gpio.c

app_objs = scan_cfg.o   modem.o main.o scan_modem.o scan_file.o scan_gps.o  scan_logic_control.o
app_src =  scan_cfg.c   modem.c main.c scan_modem.c scan_file.c scan_gps.c  scan_logic_control.c

#scan_logic_control.c

src = $(app_src) $(oss_src)
objs = $(oss_objs) $(app_objs)

INC_PATH = -I$(OSS_PATH)/ -I./active/common/ -I./

CC = mipsel-openwrt-linux-gcc -g $(INC_PATH)
app = scanner_2p

#CC = gcc -g $(INC_PATH)
#app = scanner

hp206c: $(objs)
	$(CC) $(objs) -DDEBUG  -lrt  -lpthread -lm   -o $(app)
	scp $(app) root@192.168.8.1:/root/scanner/ 

objs:$(src)
	$(CC) -o $@ -c $<

.PHONY:clean
clean: 
	rm $(app_objs)
	rm $(app)
	rm $(oss_obj) ./$(OSS_PATH)/*.o
