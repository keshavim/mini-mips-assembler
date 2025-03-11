
CC = gcc
CFLAGS = -Wall -Wextra

BIN = bin
SRC = src

all: $(BIN)/mips_converter

$(BIN)/main.o: $(SRC)/main.c
	$(CC) -o $@ -c $< $(CFLAGS)
$(BIN)/mips_converter.o: $(SRC)/mips_converter.c
	mkdir -p $(BIN)
	$(CC) -o $@ -c $< $(CFLAGS)
$(BIN)/labels.o: $(SRC)/labels.c
	$(CC) -o $@ -c $< $(CFLAGS)

$(BIN)/mips_converter: $(BIN)/mips_converter.o $(BIN)/main.o $(BIN)/strarrena.o
	$(CC) -o $@ $^ $(CFLAGS)

run: all
	./$(BIN)/mips_converter

clean:
	rm -rf $(BIN)
