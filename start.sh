#!/bin/bash

for i in {1..64}; do
    echo "datagen games 1000000 file $i.epd" | ./app > "output_$i.log" 2>&1 &
done

wait

echo "All done! Outputs saved to output_1.log through output_64.log"
