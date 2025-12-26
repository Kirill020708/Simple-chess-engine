#!/bin/bash

clean_output() {
    # Remove ANSI escape sequences, carriage returns, and keep only final output
    sed -u 's/\x1b\[[0-9;]*[a-zA-Z]//g' | \
    sed -u 's/\r/\n/g' | \
    tail -n 10  # Keep only the last 10 lines (final output)
}

for i in {1..64}; do
    rm $i.epd
    rm output_$i.log
    rm $i.txt
done
