#!/bin/bash
objdump -D -Mintel,x86-64 -b binary -m i386 $1
