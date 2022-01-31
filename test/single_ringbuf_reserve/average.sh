#! /bin/bash

# SCRIPT PARAMETERS:
# $1 = program to call `case1`, `case2`, `case3`, ...
# $2 = number of parallel stress instances.
# $3 = stress test duration.
# $4 = use dedicated cpu for each process (no, yes).
# $5 = number of times we repeat the test.
# example call: ./average.sh case4 5 10 no 3

if [[ $1 != 'case1' && $1 != 'case2' ]]
then
echo "$1"
    echo ""
    echo "Programs allowed at the moment: "
    echo "------------------------------------------"
    echo "- 'case1'"
    echo "- 'case2'"
    echo "------------------------------------------"
    exit 0
fi


if [[ -z $1 || -z $2 || -z $3 || -z $4 || -z $5 ]]
then
    echo ""
    echo "SCRIPT PARAMETERS: you must insert all these five parameters."
    echo "------------------------------------------"
    echo "1 = program to call 'case1', 'case2', 'case3', ..."
    echo "2 = number of parallel stress instances."
    echo "3 = stress test duration."
    echo "4 = use dedicated cpu for each process (no, yes)."
    echo "5 = number of times we repeat the test."
    echo "------------------------------------------"
    echo "--> example call: sudo ./average.sh case1 5 10 no 3"
    echo ""
    exit 0
fi

> "$1.txt"
repeat=$5

echo ""
echo "CONFIGURATION: "
echo "------------------------------------------"
echo "| 1. Case under test:         -> '$1'" 
echo "| 2. Number of stress-tester: ->  $2"
echo "| 3. Seconds of testing:      ->  $3s"
echo "| 4. Dedicated cpus:          -> '$4'"
echo "| 5. Number of repetition:    ->  $5"
echo "------------------------------------------"

for (( i = 0; i < $5; i++ )) 
do
    source "./case.sh" $1 $2 $3 $4
done 

# wait for the file to be written by last data
sleep  2s

# parse file and compute avarage
cnt=0
drops=0
elaborated=0

while IFS= read -r line
do
    if !((cnt % 2))
    then
        drops=$(( $drops + $line ))
    else
        elaborated=$(( $elaborated + $line ))
    fi 
    ((cnt++))
done < "$1.txt"


# print result
echo ""
echo "RESULTS: "
echo "*****************************"
echo "- Drops: $((drops / repeat))"
echo "- Elaborated: $((elaborated / repeat))"
echo "*****************************"