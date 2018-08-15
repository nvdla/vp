# NVDLA Open Source Virtual Platform
---

## NVDLA

The NVIDIA Deep Learning Accelerator (NVDLA) is a free and open architecture that promotes a standard way to design deep learning inference accelerators. With its modular architecture, NVDLA is scalable, highly configurable, and designed to simplify integration and portability. Learn more about NVDLA on the project web page.

<http://nvdla.org/>

## Online Documentation

You can find the latest NVDLA Virtual Platform documentation [here](http://nvdla.org/vp.html). This README file contains only basic information.

## Download
Git clone the repository and update the submodules. After cloning the repository, run the following command to update the submodule:
```sh
git submodule update --init --recursive
```

## Build steps

***Download and build the NVDLA CMOD***

Please refer to [Integrator's Manual](http://nvdla.org/integration_guide.html) for details on building the hardware tree, and make sure the required tools listed in [Environment Setup](http://nvdla.org/integration_guide.html#environment-setup) are installed first.

```sh
$ git clone https://github.com/nvdla/hw.git
$ cd hw
$ make
$ tools/bin/tmake -build cmod_top
```

The header files and library will be generated in ```hw/outdir/<project>/cmod/release```.

***Install required tools/libraries***

```sh
$ sudo apt-get install g++ cmake libboost-dev python-dev libglib2.0-dev libpixman-1-dev liblua5.2-dev swig libcap-dev libattr1-dev
```

***Download and install SystemC 2.3.0***

```sh
$ sudo wget -O systemc-2.3.0a.tar.gz http://www.accellera.org/images/downloads/standards/systemc/systemc-2.3.0a.tar.gz
$ tar -xzvf systemc-2.3.0a.tar.gz
$ cd systemc-2.3.0a
$ sudo mkdir -p /usr/local/systemc-2.3.0/
$ mkdir objdir
$ cd objdir
$ ../configure --prefix=/usr/local/systemc-2.3.0
$ make
$ sudo make install
```

***Cmake build***

```sh
$ cmake -DCMAKE_INSTALL_PREFIX=[install dir] -DSYSTEMC_PREFIX=[systemc prefix] -DNVDLA_HW_PREFIX=[nvdla_hw prefix] -DNVDLA_HW_PROJECT=[nvdla_hw project name]
```

```install dir``` is the installation path, ```systemc prefix``` is the SystemC installation directory, ```nvdla_hw prefix``` is the local NVDLA HW repository, and ```nvdla_hw project name``` is the NVDLA HW project name. Example:

```sh
$ cmake -DCMAKE_INSTALL_PREFIX=build -DSYSTEMC_PREFIX=/usr/local/systemc-2.3.0/ -DNVDLA_HW_PREFIX=/usr/local/nvdla/hw -DNVDLA_HW_PROJECT=nv_full
```

***Compile***

```sh
$ make
```

***Installation***

```sh
$ make install
```

## Run

```sh
$ export SC_SIGNAL_WRITE_CHECK=DISABLE
$ ./build/bin/aarch64_toplevel --conf [platform.lua]
```

```platform.lua``` is the path to your platform conf file. A demo example is in ```conf/aarch64_nvdla.lua```. You will need a linux kernel image to run the NVDLA software, please refer to [here](http://nvdla.org/vp.html#building-linux-kernel-for-nvdla-virtual-simulator) on how to build a kernel image for NVDLA.

## HW verion index

Commit info in github: 7cf6ad5a6e75f01aac1b0041288612e63a5999ac

