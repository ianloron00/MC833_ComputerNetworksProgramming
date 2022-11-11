#!/bin/bash

# GET STARTED ==> 
# Run permission bellow:
# > chmod +x run.sh
# Then, run the code:
# > ./run.sh cliente servidor


run() {
    for arg in "$@";
    do
        local file=$arg
        gcc -Wall $file.c -o $file
    done
}

run "$@"