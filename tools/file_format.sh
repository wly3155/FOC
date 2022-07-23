#!/bin/bash

echo "start"
function file_recursive () {
    echo "curr folder:"$1
    for filename in $1/*; do
        if [ -d $filename ]; then
            file_recursive $filename
        else
            filename1=$filename.tmp
            sed 's/\t/    /g' $filename >$filename1
            mv $filename1 $filename
        fi
    done
}

file_recursive $1 

