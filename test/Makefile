CURRENT_DIR = $(shell pwd)
COMPILE_DIR = $(CURRENT_DIR)/../

CFLAGS = -fPIC -g -pipe
LDFLAGS = -L$(COMPILE_DIR) -lscribe_utils -L/usr/lib/x86_64-linux-gnu -lmysqlclient -lpthread -lz -lm -ldl -lconfig -lwiringPi -lwiringPiDev
INCLUDES = -I$(COMPILE_DIR) -I/usr/include/mysql -DBIG_JOINS=1  -fno-strict-aliasing   -g -DNDEBUG

CC = gcc

all: scribe blink

blink:
	$(CC) blink.c $(CFLAGS)  $(LDFLAGS) $(INCLUDES) -o blink

scribe:
	$(CC) scribe_client.c $(CFLAGS)  $(LDFLAGS) $(INCLUDES) -o scribe_client

run:	
	LD_LIBRARY_PATH=$(COMPILE_DIR) ./scribe_client localhost 1463 test test

clean: 
	rm -f *.o scribe_client blink

