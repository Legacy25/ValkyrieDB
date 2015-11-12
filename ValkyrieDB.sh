#!/usr/bin/env bash
rm -f bin/temp.json
java -jar ./bin/MushroomCloud.jar --data ./test/data ./test/sql/tpch_schemas.sql ./test/sql/test.sql > bin/temp.json
./bin/llvmruntime < bin/temp.json