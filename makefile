GCC=gcc
CFLAGS= -Wall -Werror
TARGET = chip8
SRCS = $(wildcard *.c)
OBJS = $(SRCS: .c=.o)
CFLAGS += -Iinclude


all: $(TARGET)


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET)


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

