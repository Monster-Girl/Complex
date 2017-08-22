ROOT=$(shell pwd)
LIB=$(ROOT)/lib/lib
INCLUDE=$(ROOT)/lib/include

.PHONY:all
all:insert_cgi select_cgi login insert_login

insert_cgi:insert_cgi.cpp sql_agi.cpp
	g++ -o $@ $^ -L$(LIB)  -I$(INCLUDE) -lmysqlclient
select_cgi:select_cgi.cpp sql_agi.cpp
	g++ -o $@ $^ -L$(LIB)  -I$(INCLUDE) -lmysqlclient
login:login.cpp sql_agi.cpp
	g++ -o $@ $^ -L$(LIB) -I$(INCLUDE) -lmysqlclient
insert_login:insert_login.cpp sql_agi.cpp
	g++ -o $@ $^ -L$(LIB) -I$(INCLUDE) -lmysqlclient
.PHONY:clean
clean:
	rm insert_cgi select_cgi login insert_login
