#!/usr/bin/env bash
rm -f bin/temp.json
java -jar ./bin/MushroomCloud.jar --data ./test/data ./test/sql/tpch_schemas.sql $1 > bin/temp.json
./bin/llvmruntime < bin/temp.json