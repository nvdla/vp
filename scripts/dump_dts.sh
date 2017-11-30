#! /bin/bash

export SC_SIGNAL_WRITE_CHECK=DISABLE
./aarch64_toplevel -c conf/aarch64_nvdla_dump_dts.lua 
dtc virt.dtb -o virt.dts -I dtb -O dts
