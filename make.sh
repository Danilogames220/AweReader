#!/bin/bash
# shell script for easier project debugging

build () { cmake -B build && cmake --build build; }
run () { ./build/reader $@; }

if [ -z $1 ]; then echo "command not found"; exit 0; fi

# generate compile_commands.json
if [ $1 == "cc" ]; then cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ./; exit 0; fi

# delete build/
if [ $1 == "clear" ]; then rm -r build/; exit 0; fi

# build
if [ $1 == "build" ]; then build; exit 0; fi

# run 
if [ $1 == "run" ]; then run ${@:2}; exit 0; fi

build && run $@
