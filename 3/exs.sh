#!/bin/bash

terminalProfile="833"
compile() {
    for arg in "$@";
    do
        local file=$arg
        gcc -Wall $file.c -o $file
    done
}

_server() {
  echo "servidor: <porta $1> <backlog $2>"
  sudo ./servidor "$1" "$2"
}

_client() {
  echo "cliente: <ip $1> <porta $2>"
  ./cliente "$1" "$2"
}

h() {
  echo "./ex2.sh <PORT> <BACKLOG>"
}

ex2() {
  port=$((5000 + $RANDOM))
  ip="127.0.0.1"
  for ((backlog=0; backlog<=10; backlog+=2));
  do
    _server "$port" "$backlog" &
    gnome-terminal --tab --title="mc833" --command="bash -c '
    ./cliente '$ip' '$port' &
    ./cliente '$ip' '$port' &
    ./cliente '$ip' '$port' &
    ./cliente '$ip' '$port' &
    ./cliente '$ip' '$port' &
    ./cliente '$ip' '$port' &
    ./cliente '$ip' '$port' &
    ./cliente '$ip' '$port' &
    ./cliente '$ip' '$port' &
    ./cliente '$ip' '$port'' ";
    netstat -taulpn | grep "$port";
    kill -9 $(lsof -t -i:$port)
    port=$(($port+1));
  done
}

ex4() {
  port=$((5000 + $RANDOM))
  ip="127.0.0.1"
  backlog=0
  _server "$port" "$backlog" &
  gnome-terminal --tab --title="mc833" --command="bash -c '
  ./cliente '$ip' '$port' &
  ./cliente '$ip' '$port' &
  ./cliente '$ip' '$port' &
  ./cliente '$ip' '$port' &
  ./cliente '$ip' '$port' &
  ./cliente '$ip' '$port' &
  ./cliente '$ip' '$port' &
  ./cliente '$ip' '$port' &
  ./cliente '$ip' '$port' &
  ./cliente '$ip' '$port'' ";
  netstat -taulpn | grep "$port";
  # kill -9 $(lsof -t -i:$port);
  echo "status:"
  # killall -9 cliente
  ps aux | grep Z
}

main() {
  compile cliente servidor
  ex4
}

main "$@"