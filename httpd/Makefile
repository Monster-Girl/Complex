ROOT_PATH=$(shell pwd)
CONF=$(ROOT_PATH)/conf
PLUGIN=$(ROOT_PATH)/plugin
WWWROOT=$(ROOT_PATH)/wwwroot
FLAGS=#
LDFLAGS=-lpthread#-static
cc=gcc
bin=httpd
src=httpd.c main.c
obj=$(shell echo $(src) | sed 's/\.c/\.o/g') 

$(bin):$(obj)
	$(cc) -o $@ $^ $(LDFLAGS)
%.o:%.c
	$(cc) -c $<
.PHONY:clean
clean:
	rm *.o $(bin)

.PHONY:debug
debug:
	echo $(obj)
	echo $(src)
	echo $(ROOT_PATH)
