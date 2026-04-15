APP = avimax
SRC = $(shell find src -name '*.c')
BIN = bin/$(APP)

CFLAGS = -Wall -Wextra -std=c11 $(shell pkg-config --cflags gtk+-3.0 libcurl libcjson)
LDFLAGS = $(shell pkg-config --libs gtk+-3.0 libcurl libcjson) -pthread

all: $(BIN)

$(BIN): $(SRC)
	mkdir -p bin
	gcc $(SRC) -o $(BIN) $(CFLAGS) $(LDFLAGS)

run: all
	./$(BIN)

clean:
	rm -rf bin
