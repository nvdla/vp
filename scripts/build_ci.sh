#! /bin/bash

cmake -DCMAKE_INSTALL_PREFIX=build -DSYSTEMC_PREFIX=/usr/local/systemc-2.3.0/ -DNVDLA_HW_PREFIX=$PWD/../hw -DNVDLA_HW_PROJECT=nv_small -DCMAKE_BUILD_TYPE=Debug -DLUA_INCLUDE_DIR=/usr/local/include -DLUA_LIBRARIES=/usr/local/lib/liblua.a
make | tee make.log
make install
