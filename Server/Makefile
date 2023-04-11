CC=gcc
CFLAGS=-I.
DEPS=modbus.h
OBJ=server.o modbus.o

%.o:%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)


server:$(OBJ)
	$(CC) -o server server.o modbus.o
	
.PHONY:clean

clean:
	rm -f $(OBJ)