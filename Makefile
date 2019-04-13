all: client server

client: client.c
	gcc -o $@ client.c -lrt

server: server.c
	gcc -o $@ server.c -lrt

clean:
	rm client server