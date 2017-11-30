#! /bin/bash

export SC_SIGNAL_WRITE_CHECK=DISABLE
./build/bin/aarch64_toplevel -c conf/aarch64_nvdla.lua
