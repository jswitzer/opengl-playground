#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
if [ -z $1 ]; then
    echo "Usage $0 <target dir>"
    exit 1
fi

echo "Copying Makefiles from $DIR to $1" 
cp $DIR/Makefile $1/Makefile
cp $DIR/examples/Makefile $1/examples/Makefile
cp $DIR/deps/Makefile $1/deps/Makefile
cp $DIR/src/Makefile $1/src/Makefile

