CLIENT_FILES=main.c 
GCC_FLAGS= -std=c99 -Wall -Wextra -pedantic -O2 -pthread -Iinclude -MMD -MP

SRC_DIR = src
SRC := $(wildcard $(SRC_DIR)/*.c)

all: 
	gcc $(GCC_FLAGS) ${SRC} $(CLIENT_FILES) `pkg-config --cflags --libs gtk+-3.0` -o ../chat_client