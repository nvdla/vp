// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: gsgp2cosim.h

#ifndef GSGP2COSIM_H
#define GSGP2COSIM_H

#ifndef SC_INCLUDE_DYNAMIC_PROCESSES
#define SC_INCLUDE_DYNAMIC_PROCESSES
#endif

#include <systemc.h>
#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "shm_lib/simdb.hpp"
#include "shm_lib/shm_defs.h"

class Gsgp2cosim
    : public sc_core::sc_module 
    , public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
{
  public:
    Gsgp2cosim(sc_core::sc_module_name name, const char* shm_name, uint64_t addr_offset=0);
    ~Gsgp2cosim();
        
    SC_HAS_PROCESS(Gsgp2cosim);
        
    gs::gp::GenericSlavePort<32> gsgp_slave_port;
    void b_transact(gs::gp::GenericSlaveAccessHandle ah);
    
private:
    void end_of_elaboration();
    simdb shm_db;
    std::string shm_name_str;

    uint32_t shm_timeout;

    uint64_t addr_offset;
};

#endif
