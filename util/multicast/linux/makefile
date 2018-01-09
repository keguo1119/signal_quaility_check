objs = multicast.o  
src=$(wildcard *.c)
inc = common.h  
CC = gcc


multicast: $(objs)
	$(CC) $(objs)  -o multicast  

%.o:%.c
	$(CC) -g -o $@  -c $< 

.PHONY:clean
clean:
	rm $(objs)
	rm multicast 	

