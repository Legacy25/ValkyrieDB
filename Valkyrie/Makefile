INC_DIR	= ./include
SRC_DIR = ./src
OBJ_DIR	= ./object

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)
OBJ_PATH  = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJ_FILES))

LIBS	= $(INC_DIR)/*.h
CC	= clang++-3.5
CFLAGS	= `llvm-config-3.5 --cxxflags --ldflags --system-libs --libs core mcjit native bitwriter` -g -I$(INC_DIR)

all: valkyrie

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

valkyrie: 
	clang++-3.5 src/*.cpp -g -O0 `llvm-config-3.5 --cxxflags --ldflags --system-libs --libs core mcjit native bitwriter` -o build/llvmruntime -I./include

clean:
	rm -f $(OBJ_DIR)/*.o $(INC_DIR)/*~ valkyrie
	rm -rf build