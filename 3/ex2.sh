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

run_once() {
  port=$((5001 + $RANDOM % 5000))
  backlog=0
  ip="127.0.0.1"
  echo "server port: $port"
  _server "$port" "$backlog" &
  gnome-terminal --tab --title="mc833" --command="bash -c './cliente '$ip' '$port'; $SHELL'";
  # \ gnome-terminal --tab --title="mc833" window-with-profile="$terminalProfile" -- "_client '$ip' '$port'";
  netstat -taulpn | grep "$port"
}

solve() {
  port=$((5000 + $RANDOM))
  backlog=0
  ip="127.0.0.1"
  for ((backlog=0; backlog<=0; backlog++));
  do
    _server "$port" "$backlog" && \
    for ((n_clients=0; n_clients<1; n_clients++));
    do
      _client "$ip" "$port" & \
    done

    netstat -taulpn | grep "$port"
    port=port+1
  done
}

ex2() {
  compile cliente servidor
  # solve
  run_once
}

ex2 "$@"