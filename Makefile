CLIENT_FILES=main.c 
SERVER_FILES=main.c 
GCC_FLAGS= -std=c99 -Wall -Wextra -pedantic -O2 -pthread -Iinclude -MMD -MP

SRC_DIR = src
SRC := $(wildcard $(SRC_DIR)/*.c)

.PHONY: all clean


all: chatserver chatclient

chatserver:
	cd server; make
chatclient:
	cd client; make
clean:
	rm chat_client chat_server 