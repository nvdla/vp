// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: extmem_cosim_sc_wrapper.h

#ifndef EXTMEM_COSIM_SC_WRAPPER_H
#define EXTMEM_COSIM_SC_WRAPPER_H

#include <systemc.h>
#include "gsgpsocket/transport/GSGPSlaveSocket.h"

#include "shm_lib/simdb.hpp"

class Gsgp2cosim;

class extmem_cosim_sc_wrapper 
    : public sc_module
    , public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
{
public:
    extmem_cosim_sc_wrapper(sc_core::sc_module_name name);
    ~extmem_cosim_sc_wrapper();
    
    gs::gp::GenericSlavePort<32> target_port;
    
    void b_transact(gs::gp::GenericSlaveAccessHandle ah) {};

private:
    gs::gs_param<uint32_t> m_size;

    Gsgp2cosim * m_gsgp2cosim;

    void end_of_elaboration(void);
};

#endif /* !EXTMEM_COSIM_SC_WRAPPER_H */
