#!/usr/bin/env bash

PORTS=(4243 4244 4245)
METHODS=(GET POST)
PATHS=("/" "/test" "/invalid")
MAX_CLIENTS=100

send_request() {
  local port=${PORTS[$RANDOM % ${#PORTS[@]}]}
  local method=${METHODS[$RANDOM % ${#METHODS[@]}]}
  local path=${PATHS[$RANDOM % ${#PATHS[@]}]}

  if [[ "$method" == "GET" ]]; then
    curl -s --max-time 1 \
      "http://127.0.0.1:${port}${path}" \
      -o /dev/null
  else
    curl -s --max-time 1 \
      -X POST \
      -H "Content-Type: text/plain" \
      --data "ping" \
      "http://127.0.0.1:${port}${path}" \
      -o /dev/null
  fi
}

for ((i=0; i<MAX_CLIENTS; i++)); do
  send_request &
  sleep 0.01   # prevent FD burst
done

wait
echo "Done"
