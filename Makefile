CC = gcc
SRC = Chippy-8.c
TARGET = Chippy-8
CFLAGS = -std=c11 -pedantic -Wall -Wextra -flto -march=native -mtune=native -O2
LDFLAGS = -lSDL2

$(TARGET):	$(SRC)
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(TARGET)
