rm -f bin/temp.json
java -jar ./bin/MushroomCloud.jar --data $1 $2 > bin/temp.json
./bin/valkyrie < bin/temp.json