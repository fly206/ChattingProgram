client server: client.c server.c
	gcc -o client client.c
	gcc -o server server.c

clean:
	-rm -rf client server

