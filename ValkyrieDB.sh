rm -f bin/temp.json
java -jar ./bin/MushroomCloud.jar --data /home/vinayak/CourseWork/Databases/PA2data/data/ /home/vinayak/CourseWork/Databases/PA3data/test.sql > bin/temp.json
./bin/valkyrie < bin/temp.json