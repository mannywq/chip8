GCC=gcc
CFLAGS= -Wall -Werror -g
TARGET = chip8
SRCS = $(wildcard *.c)
OBJS = $(SRCS: .c=.o)
CFLAGS += -Iinclude
CFLAGS += -I/opt/homebrew/include/SDL2 -D_THREAD_SAFE
LIBS += -L/opt/homebrew/lib -lSDL2

all: $(TARGET)


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $(TARGET) $(LIBS)


%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

