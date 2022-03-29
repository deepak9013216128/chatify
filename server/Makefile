SERVER_FILES=main.c 
GCC_FLAGS= -std=c99 -Wall -Wextra -pedantic -O2 -pthread -Iinclude -MMD -MP

SRC_DIR = src
SRC := $(wildcard $(SRC_DIR)/*.c)

all: 
	gcc $(GCC_FLAGS) $(SRC)  $(SERVER_FILES) -lsqlite3 -lssl -lcrypto -o ../chat_server