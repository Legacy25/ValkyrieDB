cd Valkyrie
clang++-3.5 src/*.cpp -g -O0 `llvm-config-3.5 --cxxflags --ldflags --system-libs --libs core mcjit native bitwriter` -o ../bin/llvmruntime -I./include
cd ..
rm -f bin/temp.json
java -jar ./bin/MushroomCloud.jar --data /home/vinayak/CourseWork/Databases_and_Programming_Languages/Project/CDB/CDB/data /home/vinayak/CourseWork/Databases/PA3data/test.sql > bin/temp.json
./bin/llvmruntime < bin/temp.json