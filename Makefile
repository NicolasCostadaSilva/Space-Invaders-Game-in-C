CC=gcc
CFLAGS=-Wall -Wextra -Wno-switch
LDFLAGS=$(shell pkg-config allegro-5 allegro_font-5 allegro_primitives-5 allegro_audio-5 allegro_acodec-5 allegro_image-5 --libs --cflags)

SRC=main.c geral.c exibe.c teclado.c audio.c
TARGET=ncs22-A4

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
