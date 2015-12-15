#!/bin/bash
echo "SQLITE"
time sqlite3 test/tpch.db < $1 > /dev/null

echo "VALKYRIE"
rm -f bin/temp.json
java -jar ./bin/MushroomCloud.jar --data ./test/data ./test/sql/tpch_schemas.sql $1 > bin/temp.json
time ./bin/llvmruntime -nollvm < bin/temp.json > /dev/null

echo "MUSHROOM-CLOUD (PROJECTION AWARE SCAN)"
time java -jar bin/MushroomCloud.jar --data test/data/ --exec --pdp test/sql/tpch_schemas.sql $1 > /dev/null 

echo "MUSHROOM-CLOUD"
time java -jar bin/MushroomCloud.jar --data test/data/ --exec test/sql/tpch_schemas.sql $1 > /dev/null 
