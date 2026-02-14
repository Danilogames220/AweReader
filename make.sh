#!/bin/bash
# simple shell script to do stuff to the project that i dont want to type everytime i open a new terminal session
# please run this only when you are on the current dir of this script

build () { cmake -B build && cmake --build build; }
run () { ./build/reader; }

if [ -z $1 ]; then build && run; exit 0; fi

# generate compile_commands.json
if [ $1 == "cc" ]; then cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ./; fi

# delete build/
if [ $1 == "clear" ]; then rm -r build/; fi

# build
if [ $1 == "build" ]; then build; fi

# run 
if [ $1 == "run" ]; then run; fi
#$build_command # build when no arguments are passed
