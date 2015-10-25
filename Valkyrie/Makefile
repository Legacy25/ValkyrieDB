INC_DIR	= ./include
SRC_DIR = ./src
OBJ_DIR	= ./object

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)
OBJ_PATH  = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJ_FILES))

LIBS	= 
CC	= /usr/bin/g++
CFLAGS	= -g -I$(INC_DIR)

all: valkyrie

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

valkyrie: $(OBJ_PATH)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(OBJ_DIR)/*.o $(INC_DIR)/*~ valkyrie