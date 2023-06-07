EXEC_NAME = server
CC=gcc
CFLAGS=-I.
DEPS=modbus.h sqlite3.h
OBJ=server.o modbus.o sqlite3.o 
FLAG = -lpthread -ldl

%.o:%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(FLAG)


server:$(OBJ)
	$(CC) -o $(EXEC_NAME) server.o modbus.o sqlite3.o $(FLAG)
	
.PHONY:clean

clean:
	rm -f $(OBJ)
	rm Transit.db