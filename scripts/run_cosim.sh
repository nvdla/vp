#! /bin/bash

export SC_SIGNAL_WRITE_CHECK=DISABLE
./build/bin/aarch64_toplevel -c conf/aarch64_nvdla.lua --dmi_disable --cosim --simv fpga/aws-fpga/cl_nvdla/verif/sim/cosim_test --plusargs +enable_sim_mem
