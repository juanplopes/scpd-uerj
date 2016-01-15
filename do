#!/bin/bash
set -e 

if [ $# -lt 1 ]; then
    echo 'compile | run <tasks> | html '
    exit 0
fi

while test -n "$1"
do
    case "$1" in
        compile)
            echo "mpic++.mpich main.cpp -lpthread -lX11 -std=c++11"
            mpic++.mpich main.cpp -lpthread -lX11 -std=c++11
    ;;
        test)
            echo "mpirun.mpich -n $2 ./a.out"
            mpirun.mpich -n $2 ./a.out
            shift
    ;;
        run)
            echo "mpirun.mpich -n $2 ./a.out > out.txt"
            mpirun.mpich -n $2 ./a.out > out.txt
            shift
    ;;
        html)
            echo "./display < out.txt > test.html"
            ./display < out.txt > test.html
            echo "sensible-browser test.html"
            sensible-browser test.html
    ;; join)
        mkdir -p images
        mv node*/* images/
        rm -rf node*/
    ;; split)
        for file in images/*
        do
            DIR="node$((RANDOM % $2 + 1))"
            mkdir -p $DIR
            mv "$file" "$DIR"
        done
        shift
        rm -rf images/
    ;;
    
    esac
    shift
done

