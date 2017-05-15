.PHONY:All
All:client server

client:client.c comm.c
	gcc -o $@ $^
server:server.c comm.c
	gcc -o $@ $^

.PHONY:clean
clean:
	rm -rf server client
