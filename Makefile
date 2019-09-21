all:
	gcc *.c  `pkg-config --cflags --libs ` -lpthread -g -o ChartRoomServer

clean:
	rm -rf *.o MyfirstServer
