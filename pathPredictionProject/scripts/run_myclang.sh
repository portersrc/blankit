#!/bin/bash -e

FILES=./*.c
DIR=$1
FILES=$(find $(DIR) -type f -name '*.c')
for f in $FILES
do
  echo "Processing $f file..."
   clang-instrument $f > t.c
   if [ -s t.c ] 
   then 
	echo "$f is nonempty file"
	mv t.c $f
   fi
  # take action on each file. $f store current file name
   #rm t.c
done
