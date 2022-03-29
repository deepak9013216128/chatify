CLIENT_FILES=main.c 
GCC_FLAGS= -std=c99 -w -pthread -Iinclude -MMD -MP

SRC_DIR = src
SRC := $(wildcard $(SRC_DIR)/*.c)

all: 
	gcc $(GCC_FLAGS) ${SRC} $(CLIENT_FILES) `pkg-config --cflags --libs gtk+-3.0` -lssl -lcrypto -o ../chat_client