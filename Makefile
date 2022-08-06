SRC = Chippy-8.c
TARGET = Chippy-8
CFLAGS = -std=c11 -Wall -Wextra -flto -march=native -mtune=native -O2

$(TARGET):	$(SRC)
	gcc $(SRC) -lSDL2 $(CFLAGS) -o $(TARGET)