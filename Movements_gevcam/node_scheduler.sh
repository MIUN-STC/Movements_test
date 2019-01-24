#!/bin/sh
cd /home/ubuntu/Movements2
echo "" >> record.log
echo `date ` >> record.log
echo "" >> record.log
./bin/test_record -b20 -B500000 -n60 -t60 -o /mnt/ssd/vids/node2 >> record.log 2>&1
