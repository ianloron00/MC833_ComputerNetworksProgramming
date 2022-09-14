#!/bin/bash

# GET STARTED ==> 
# Run permission bellow:
# > chmod +x bash.sh
# Then, run the code:
# > ./bash.sh < function<parameters> >
# Examples:
# "./bash.sh run cliente"
# "./bash.sh del servidor"

compile() {
    if [ "$1" == "run" ]
    then 
        run "${@:2}"
    elif [ "$1" == "del" ] 
    then
        del "${@:2}"
    fi
}

run() {
    local file=$1
    echo "Running $file.c ..."
    gcc -Wall $file.c -o $file
    ./$file "${@:2}"
    del $file
}

# you can pass 1+ files to this function
del() {
    echo "Deleting <$@> ..."
    for arg in "$@"; 
    do
        rm -f $arg; 
    done 
}

compile "$@"