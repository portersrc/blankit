#!/bin/bash

i=1
while [ "$i" -le "$1" ]; do
    (ssh sloppyjoe 'bash -s' < $2) &> /dev/null &
    i=$(($i + 1))
done

wait
