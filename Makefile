INC_DIR	= Valkyrie/include
SRC_DIR = Valkyrie/src
OBJ_DIR	= Valkyrie/object

SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES = $(SRC_FILES:.cpp=.o)
OBJ_PATH  = $(patsubst $(SRC_DIR)/%,$(OBJ_DIR)/%,$(OBJ_FILES))

LIBS	= 
CC	= /usr/bin/g++
CFLAGS	= -g -I$(INC_DIR)

CC=/usr/bin/g++
CFLAGS=-g -I$(INC_DIR)

all: init mushroomcloud valkyrie

init:
	mkdir -p ./bin
	mkdir -p Valkyrie/object
	mkdir -p MushroomCloud/bin

mushroomcloud:
	javac -d MushroomCloud/bin/ -cp MushroomCloud/src:"MushroomCloud/lib/*" -sourcepath MushroomCloud/src MushroomCloud/src/edu/buffalo/cse562/Main.java
	jar cvfm bin/MushroomCloud.jar MushroomCloud/Manifest.txt -C MushroomCloud/bin/ .
	cp -a MushroomCloud/lib/. ./bin/

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

valkyrie: $(OBJ_PATH)
	$(CC) -o bin/$@ $^ $(CFLAGS) $(LIBS)

clean:
	rm -r MushroomCloud/bin
	rm -r ./bin
	rm -r Valkyrie/object