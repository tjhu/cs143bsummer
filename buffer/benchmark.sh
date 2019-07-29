#!/bin/bash
# See the main function in circular_buffer_main.cpp for usage
trap 'exit 130' INT

function run() {
    /usr/bin/time -f "time: %e\n" ./circular_buffer_main $1 $2 $3 $4 $5 $6
}

for i in {1..10..1}
  do 
    echo "======iteration $i======="
    run $i 1 1 2 10 10          2>&1
 done