#!/bin/bash

# step 1
./bootstrap.sh

# step 2
export freeswitch_base_path=$(pwd)
./configure --prefix=$freeswitch_base_path/All/Debug

# step 3
make clean && make && make install