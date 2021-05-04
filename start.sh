#!/bin/bash

make clean
make
clear

if ! command -v fzf &> /dev/null
then
    echo "fzf could not be found"
    exit
else
    ./super-fs fs $(fzf --header="Please choose FS-file")
fi
