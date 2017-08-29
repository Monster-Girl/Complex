ROOT_PATH=$(shell pwd)
CONF=$(ROOT_PATH)/conf
PLUGIN=$(ROOT_PATH)/plugin
SQL=$(ROOT_PATH)/sql_connect
FLAGS=#
LDFLAGS=-lpthread#-static
cc=gcc
bin=httpd
src=httpd.c main.c
obj=$(shell echo $(src) | sed 's/\.c/\.o/g') 
.PHONY:all
all:$(bin) cgi
$(bin):$(obj)
	$(cc) -o $@ $^ $(LDFLAGS)
%.o:%.c
	$(cc) -c $<
cgi:
	cd $(SQL);\
		make;\
		cd -
.PHONY:clean
clean:
	rm *.o $(bin)
	cd $(SQL);\
		make clean;\
		cd -
.PHONY:output
output:
	mkdir -p output/conf
	cp httpd output

