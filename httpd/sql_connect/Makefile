ROOT=$(shell pwd)
LIB=$(ROOT)/lib/lib
INCLUDE=$(ROOT)/lib/include

.PHONY:all
all:select_cgi login insert_login insert_info

select_cgi:select_cgi.cpp sql_agi.cpp
	g++ -o $@ $^ -L$(LIB)  -I$(INCLUDE) -lmysqlclient
login:login.cpp sql_agi.cpp
	g++ -o $@ $^ -L$(LIB) -I$(INCLUDE) -lmysqlclient
insert_login:insert_login.cpp sql_agi.cpp
	g++ -o $@ $^ -L$(LIB) -I$(INCLUDE) -lmysqlclient
insert_info:insert_info.cpp sql_agi.cpp
	g++ -o $@ $^ -L$(LIB) -I$(INCLUDE) -lmysqlclient
.PHONY:clean
clean:
	rm select_cgi login insert_login insert_info
