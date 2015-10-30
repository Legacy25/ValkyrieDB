rm -f bin/temp.json
java -jar ./bin/MushroomCloud.jar --data ./test/data ./test/sql/test.sql > bin/temp.json
./bin/llvmruntime < bin/temp.json