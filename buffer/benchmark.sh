function run() {
    /usr/bin/time -f %e ./circular_buffer_main $1 $2 $3 $4 > /dev/null
}

for i in {1..10..1}
  do 
     printf "n=$i: %s\n" $(run $i 10 10 2 2>&1)
 done