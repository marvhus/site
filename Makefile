BIN := tools-bin
OBJ := $(BIN)/obj
SRC := tools

CXX      := g++
CXXFLAGS := -ggdb -Wall -Wextra -Werror -Wpedantic -std=c++20 -pedantic -I$(SRC)
CC      := gcc
CCFLAGS := -ggdb -Wall -Wextra -Werror -Wpedantic -std=c11 -pedantic -I$(SRC)

.PHONY: all clean

all: $(BIN)/builder

clean:
	rm -rf $(BIN)

$(BIN)/builder: $(OBJ)/builder.o $(BIN)/md4c-html.a
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)/builder.o -L$(BIN) -lmd4c-html

$(OBJ)/builder.o: $(SRC)/builder.cpp
	@mkdir -p $(shell dirname $@)
	$(CXX) $(CXXFLAGS) -o $@ -c $(SRC)/builder.cpp

# MD4C
$(BIN)/md4c-html.a: $(OBJ)/md4c/md4c.o $(OBJ)/md4c/md4c-html.o $(OBJ)/md4c/entity.o
	@mkdir -p $(shell dirname $@)
	ar cr $@ $(OBJ)/md4c/md4c.o $(OBJ)/md4c/md4c-html.o $(OBJ)/md4c/entity.o

$(OBJ)/md4c/md4c.o: $(SRC)/md4c/md4c.c $(SRC)/md4c/md4c.h
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -o $(OBJ)/md4c/md4c.o -c $(SRC)/md4c/md4c.c

$(OBJ)/md4c/md4c-html.o: $(SRC)/md4c/md4c-html.c $(SRC)/md4c/md4c-html.h
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -o $(OBJ)/md4c/md4c-html.o -c $(SRC)/md4c/md4c-html.c

$(OBJ)/md4c/entity.o: $(SRC)/md4c/entity.c $(SRC)/md4c/entity.h
	@mkdir -p $(shell dirname $@)
	$(CC) $(CFLAGS) -o $(OBJ)/md4c/entity.o -c $(SRC)/md4c/entity.c
#######
