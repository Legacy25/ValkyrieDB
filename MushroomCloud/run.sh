#!/bin/sh
SCRIPT=$(readlink -f "$0")
SCRIPTPATH=$(dirname "$SCRIPT")
cd $SCRIPTPATH
java -XX:+UseCompressedOops -Xms512m -Xmx2048m -classpath bin:"lib/*" edu.buffalo.cse562.Main "$@"
