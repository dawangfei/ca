#!/bin/ksh


num=1
num=10
num=100
num=1000

i=0
while (( ((i = i + 1)) <= ${num}))
do
    ./mix > ben.${i}.log 2>&1 &
done

echo "total [${num}] worker"

wait

b.awk ben*log


#
