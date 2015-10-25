CC=clang++-3.5
CFLAGS=-I Valkyrie/include

all: mushroomcloud valkyrie

mushroomcloud:
	mkdir -p ./bin
	mkdir -p MushroomCloud/bin
	javac -d MushroomCloud/bin/ -cp MushroomCloud/src:"MushroomCloud/lib/*" -sourcepath MushroomCloud/src MushroomCloud/src/edu/buffalo/cse562/Main.java
	jar cvfm bin/MushroomCloud.jar MushroomCloud/Manifest.txt -C MushroomCloud/bin/ .
	cp -a MushroomCloud/lib/. ./bin/

valkyrie:
	$(CC) -g -O3 Valkyrie/src/Main.cpp `llvm-config --cxxflags --ldflags --system-libs --libs core` -o ./bin/valkyrie $(CFLAGS)

clean:
	rm -r MushroomCloud/bin
	rm -r ./bin