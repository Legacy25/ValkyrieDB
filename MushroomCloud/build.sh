#!/bin/sh
# Absolute path to this script, e.g. /home/user/bin/foo.sh
SCRIPT=$(readlink -f "$0")
# Absolute path this script is in, thus /home/user/bin
SCRIPTPATH=$(dirname "$SCRIPT")
cd $SCRIPTPATH

mkdir -p bin
javac -d bin -cp "lib/*" -sourcepath src src/edu/buffalo/cse562/Main.java
