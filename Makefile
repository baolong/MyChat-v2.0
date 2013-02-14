SRC = ./src/

a:$(SRC)server.c $(SRC)client.c $(SRC)windows.c $(SRC)user.c $(SRC)user.h $(SRC)net.c $(SRC)net.h $(SRC)windows.h $(SRC)include.h
	gcc -g -o a $(SRC)server.c $(SRC)windows.c $(SRC)windows.h $(SRC)user.h $(SRC)user.c $(SRC)net.h $(SRC)net.c -lcursesw -lpthread
	gcc -g -o b $(SRC)client.c $(SRC)windows.c $(SRC)windows.h $(SRC)user.h $(SRC)user.c $(SRC)net.h $(SRC)net.c -lcursesw -lpthread
c:
	rm a b
