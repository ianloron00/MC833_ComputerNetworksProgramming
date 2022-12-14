#!/bin/bash

# GET STARTED ==> 
# Run permission bellow:
# > chmod +x run.sh
# Then, run the code:
# > ./run.sh cliente servidor


compile() {
    for arg in "$@";
    do
        local file=$arg
        gcc -Wall $file.c -o $file
    done
}

client() {
  ip="127.0.0.1"
  ./cliente $ip $1 $2 "<" "in.txt" ">" "out.txt"
}

server() {
  # port=$((5000 + $RANDOM))
  sudo ./servidor $1
}

compile cliente servidor
"$@"