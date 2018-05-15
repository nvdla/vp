#! /bin/bash

cmake -DCMAKE_INSTALL_PREFIX=build -DSYSTEMC_PREFIX=/usr/local/systemc-2.3.0/ -DNVDLA_HW_PREFIX=$PWD/../hw -DNVDLA_HW_PROJECT=nv_small -DCMAKE_BUILD_TYPE=Debug
make | tee make.log
make install
