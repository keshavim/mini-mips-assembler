

CFLAGS = -Wall -Wextra

BIN = bin

all: $(BIN)/mips_converter

$(BIN)/main.o: main.c
	gcc -o $@ -c $< $(CFLAGS)
$(BIN)/mips_converter.o: mips_converter.c
	mkdir -p $(BIN)
	gcc -o $@ -c $< $(CFLAGS)

$(BIN)/mips_converter: $(BIN)/mips_converter.o $(BIN)/main.o
	gcc -o $@ $^ $(CFLAGS)

run: all
	./$(BIN)/mips_converter

clean:
	rm -rf $(BIN)
