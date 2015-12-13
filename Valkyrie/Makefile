INC_DIR	= ./include
SRC_DIR = ./src
OBJ_DIR	= ./object

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)
OBJ_PATH  = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJ_FILES))

LIBS= 
CC=clang++-3.5
CFLAGS=-g -O3 `llvm-config-3.5 --cxxflags --ldflags --system-libs --libs core mcjit native bitwriter`

all: valkyrie

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

valkyrie: 
	mkdir -p build
	$(CC) $(SRC_DIR)/*.cpp $(CFLAGS) -o build/llvmruntime -I$(INC_DIR)

clean:
	rm -f $(OBJ_DIR)/*.o $(INC_DIR)/*~ valkyrie
	rm -rf build