#! /bin/bash

# SCRIPT PARAMETERS:
# $1 = program to call `case1`, `case2`, `case3`, ...
# $2 = number of parallel stress instances.
# $3 = stress test duration.
# $4 = use dedicated cpu for each process (no, yes).

# example call: ./case.sh case4 5 10 no

# kill all childs in case of premature death.
trap "pkill -P $$; kill -INT $$" INT


if [ $4 = "yes" ]
then
    # cpu 2
    taskset 0x04 "./../../single_ringbuf_reserve/$1" >> "$1.txt" &
    sleep  1s
    # cpu 2 is left for the userspace program, cpu 0, cpu 1, cpu 3, ... are used for the stress test.
    while read n
    do
        if [[ $n -gt 2 ]]
        then
        n=$((2 ** $n))
        fi
        taskset "$(printf '0x%02x' $n)" ./../stress_test &
    done < <(seq "$2")
else
    "./../../single_ringbuf_reserve/$1" >> "$1.txt" &
    sleep  1s
    while read n
    do
        ./../stress_test &
    done < <(seq "$2")
fi    

sleep "$3"s
pkill -2 -P $$ 