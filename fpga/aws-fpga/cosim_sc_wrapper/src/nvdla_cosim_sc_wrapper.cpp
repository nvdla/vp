// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: nvdla_cosim_sc_wrapper.cpp

#include "cosim_sc_wrapper/nvdla_cosim_sc_wrapper.h"
#include "transactors/gsgp2cosim.h"
#include "transactors/irq2gss.h"
#include "SimpleCPU/IRQ.h"

using namespace sc_core;

nvdla_cosim_sc_wrapper::nvdla_cosim_sc_wrapper(sc_module_name name):
    target_port("csb_port"),
    irq_socket("irq_socket"),
    irqNumber("irq_number", 0),
    m_size("size", 0x100)
{
    gs::socket::config<gs_generic_signal::gs_generic_signal_protocol_types> cnf;
    cnf.use_mandatory_extension<IRQ_LINE_EXTENSION>();
    irq_socket.set_config(cnf);

    std::stringstream apb_db_str;
    pid_t nPID_host = getpid();
    apb_db_str << "apb_ipc_db_" << nPID_host;

    std::stringstream irq_db_str;
    irq_db_str << "irq_ipc_db_" << nPID_host;

    // Create adaptors
    m_gsgp2cosim = new Gsgp2cosim("csb_adaptor", const_cast<char *>(apb_db_str.str().c_str()));
    m_irq2gss = new irq2gss("irq_adaptor", (uint32_t)irqNumber, const_cast<char *>(irq_db_str.str().c_str()));
    
    // Bind CSB port
    this->target_port.bind( m_gsgp2cosim->gsgp_slave_port );
    
    // Bind IRQ port
    m_irq2gss->m_socket.bind( irq_socket );

}

/**
 * Destructor
 */
nvdla_cosim_sc_wrapper::~nvdla_cosim_sc_wrapper()
{
    if (m_gsgp2cosim)
        delete m_gsgp2cosim;
    if (m_irq2gss)
        delete m_irq2gss;
}

/*
 *
 * End of elaboration
 */
void nvdla_cosim_sc_wrapper::end_of_elaboration()
{
}
