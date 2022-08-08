all:
	gcc -o client client.c
	gcc -o equipInfo equipInfo.c
	gcc -o server server.c
	gcc -o systemInfo systemInfo.c
	gcc -o writeLogProcess writeLogProcess.c
