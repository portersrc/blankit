#!/bin/bash

# Use on remote machine like so:
#   ssh sloppyjoe 'bash -s' < a.sh

echo "hello world" > hello.txt
cat hello.txt
