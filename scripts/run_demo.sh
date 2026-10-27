#!/bin/sh
set -e
cd /home/claude/tcp-lb

for p in 8001 8002 8003 8004; do
  ./server "$p" > "/tmp/s_$p.log" 2>&1 &
done
sleep 1

./lb linear > /tmp/lb_linear.log 2>&1 &
LB_PID=$!
sleep 1

./client 1 10 > /dev/null
./client 2 10 > /dev/null
./client 3 20 > /dev/null
./client 4 30 > /dev/null
./client 42 10 > /dev/null
sleep 1

kill "$LB_PID" 2>/dev/null || true
sleep 1

./lb finger > /tmp/lb_finger.log 2>&1 &
LB_PID=$!
sleep 1

./client 1 10 > /dev/null
./client 2 10 > /dev/null
./client 3 20 > /dev/null
./client 4 30 > /dev/null
./client 42 10 > /dev/null
sleep 1

kill "$LB_PID" 2>/dev/null || true
pkill -f './server' 2>/dev/null || true

echo "=== LB linear mode ==="
cat /tmp/lb_linear.log
echo ""
echo "=== LB finger mode ==="
cat /tmp/lb_finger.log
