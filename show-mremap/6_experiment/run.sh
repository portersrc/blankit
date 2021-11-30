#!/bin/bash

cp libfoo/libfoo.so mark_writable/libfoo.so
cp libfoo/libfoo.so mark_writable/libfoo_backup.so
mark_writable/mark_writable mark_writable/libfoo.so

./a.out
