ROOT_PATH=$(shell pwd)
CONF=$(ROOT_PATH)/conf
PLUGIN=$(ROOT_PATH)/plugin
WWWROOT=$(ROOT_PATH)/wwwroot
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
	cd $(WWWROOT)/cgi-bin;\
		make;\
		cd -
.PHONY:clean
clean:
	rm *.o $(bin)
	cd $(WWWROOT)/cgi-bin;\
		make clean;\
		cd -
	cd $(SQL);\
		make clean;\
		cd -
.PHONY:output
output:
	mkdir -p output/wwwroot/cgi-bin
	mkdir -p output/conf
	cp httpd output
	cp math_cgi out/wwwroot/cgi-bin

