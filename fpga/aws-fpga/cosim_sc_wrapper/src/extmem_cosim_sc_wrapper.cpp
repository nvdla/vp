// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: extmem_cosim_sc_wrapper.cpp

#include "cosim_sc_wrapper/extmem_cosim_sc_wrapper.h"
#include "transactors/gsgp2cosim.h"
#include "transactors/irq2gss.h"

using namespace sc_core;

extmem_cosim_sc_wrapper::extmem_cosim_sc_wrapper(sc_module_name name):
    target_port("target_port"),
    m_size("size", 0x100)
{
    std::stringstream db_str;
    pid_t nPID_host = getpid();
    db_str << "ram_ipc_db_" << nPID_host;

    // Create adaptors
    m_gsgp2cosim = new Gsgp2cosim("extmem_adaptor", const_cast<char *>(db_str.str().c_str()), 0xC0000000);

    // Bind CSB port
    this->target_port.bind( m_gsgp2cosim->gsgp_slave_port );
}

/**
 * Destructor
 */
extmem_cosim_sc_wrapper::~extmem_cosim_sc_wrapper()
{
    if (m_gsgp2cosim)
        delete m_gsgp2cosim;
}

/*
 *
 * End of elaboration
 */
void extmem_cosim_sc_wrapper::end_of_elaboration()
{
}
