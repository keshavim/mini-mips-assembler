
CC = gcc
CFLAGS = -Wall -Wextra

TARGET = program

BIN = bin
SRC = src

SRC_FLS = $(wildcard $(SRC)/*.c)

OBJ_FLS = $(patsubst $(SRC)/%.c,$(BIN)/%.o,$(SRC_FLS))

all: $(BIN) $(BIN)/$(TARGET)

$(BIN):
	mkdir -p $(BIN)

$(BIN)/$(TARGET): $(OBJ_FLS) | $(BIN)
	$(CC) -o $@ $^ $(CFLAGS)

$(BIN)/%.o: $(SRC)/%.c | $(BIN)
	$(CC) -o $@ -c $< $(CFLAGS)

run: all
	./$(BIN)/$(TARGET)

clean:
	rm -rf $(BIN)


.PHONY: all run clean
