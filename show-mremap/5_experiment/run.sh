#!/bin/bash

cp libfoo/libfoo.so mark_writable
mark_writable/mark_writable mark_writable/libfoo.so

./a.out
