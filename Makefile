CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11 -g

TARGET = main
SRC = main.c
HDR = ppm_image.h

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CC) $(CFLAGS) -o $@ $(SRC)

clean:
	rm -f $(TARGET) output.ppm

.PHONY: all clean
