#!/bin/bash

run() {
    for arg in "$@";
    do
        local file=$arg
        gcc -Wall $file.c -o $file
    done
}

_server() {
  # echo "servidor: <porta> <backlog>"
  sudo ./servidor 127.0.0.1 "$1"
}

_client() {
  # echo "cliente: <ip> <porta>"
  ./cliente "$1" "$2"
}

h() {
  echo "./ex2.sh <PORT> <BACKLOG>"
}

solve() {
  port=$((5000 + $RANDOM))
  backlog=0
  for ((backlog=0; backlog<=0; backlog++));
  do
    _server "$port" "$backlog" & \
    for ((n_clients=0; n_clients<1; n_clients++));
    do
      _client "$ip" "$port" & \
    done

    netstat -taulpn | grep "$port"
    port=port+1
  done
}

ex2() {
  run cliente servidor
  solve
  # if [ $# -ne 2 ]
  # then
  #   echo "erro de input. rode: " h
  #   echo "entrada: $@"
  # else
  #   run cliente servidor
  #   solve
  #   # _server "$1" "$2" & _client "$1" & netstat -taulpn | grep "$1"
  # fi
}

ex2 "$@"