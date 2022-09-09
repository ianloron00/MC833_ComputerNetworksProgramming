#!/bin/bash

# GET STARTED ==> 
# Run permission permission bellow:
# > chmod +x compile.sh
# Then, run the code:
# > ./compile.sh < function<parameters> >
# Examples:
# "./compile.sh run cliente"
# "./complile.sh del servidor"

compile() {
    if [ "$1" == "run" ]
    then 
        run "$2"
    elif [ "$1" == "del" ] 
    then
        del "${@:2}"
    fi
}

run() {
    local file=$1
    gcc -Wall $1.c -o $file
    ./$file
    del $file
}

# you can pass 1+ files to this function
del() {
    echo "deleting:"
    for arg in "$@"; do
        echo "<$arg>";
        rm -f $arg; 
    done 
}

compile "$@"