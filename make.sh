#!/bin/bash
# simple shell script to do stuff to the project that i dont want to type everytime i open a new terminal session
# please run this only when you are on the current dir of this script

build () { cmake -B build && cmake --build build; }
run () { ./build/reader $@; }

if [ -z $1 ]; then build && run ; exit 0; fi

# generate compile_commands.json
if [ $1 == "cc" ]; then cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ./; exit 0; fi

# delete build/
if [ $1 == "clear" ]; then rm -r build/; exit 0; fi

# build
if [ $1 == "build" ]; then build; exit 0; fi

# run 
if [ $1 == "run" ]; then run ${@:2}; exit 0; fi

build && run $@
