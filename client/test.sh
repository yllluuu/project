#!/bin/bash 

gcc -shared -fPIC -o libsqlt.so sqlt.c -I ../../sqlite/include ;
gcc main.c -o main -I ./ -lsqlt -L ./ -I ../../sqlite/include -lsqlite3 -L ../../sqlite/lib ;

./main -i 127.0.0.1 -p 15243 -t 2 ;
