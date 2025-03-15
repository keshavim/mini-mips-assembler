
CC = gcc
DFLAGS = -MMD -MP
CFLAGS = -Wall -Wextra $(DFLAGS)

TARGET = program

BIN = bin
SRC = src
OBJ = $(BIN)/obj

SRC_FLS := $(wildcard $(SRC)/*.c)

OBJ_FLS := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRC_FLS))
DEP_FLS := $(patsubst $(SRC)/%.c,$(OBJ)/%.d,$(SRC_FLS))

all: $(BIN) $(BIN)/$(TARGET)

$(BIN):
	mkdir -p $(BIN)
	mkdir -p $(OBJ)
	mkdir -p $(DEP)

$(BIN)/$(TARGET): $(OBJ_FLS) | $(BIN)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ)/%.o: $(SRC)/%.c | $(BIN)
	$(CC) -o $@ -c $< $(CFLAGS)

run: all
	./$(BIN)/$(TARGET)

clean:
	rm -rf $(BIN)

-include $(DEP_FLS)
.PHONY: all run clean
