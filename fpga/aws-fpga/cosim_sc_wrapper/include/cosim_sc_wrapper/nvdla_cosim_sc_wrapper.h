// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: nvdla_cosim_sc_wrapper.h

#ifndef NVDLA_COSIM_SC_WRAPPER_H
#define NVDLA_COSIM_SC_WRAPPER_H

#include <systemc.h>
#include "greensignalsocket/green_signal.h"
#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#ifndef SC_INCLUDE_DYNAMIC_PROCESSES
#define SC_INCLUDE_DYNAMIC_PROCESSES
#endif

class Gsgp2cosim;
class irq2gss;

class nvdla_cosim_sc_wrapper 
    : public sc_module
    , public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
{
public:
    nvdla_cosim_sc_wrapper(sc_core::sc_module_name name);

    SC_HAS_PROCESS(nvdla_cosim_sc_wrapper);

    ~nvdla_cosim_sc_wrapper();
    
    gs::gp::GenericSlavePort<32> target_port;
    gs_generic_signal::initiator_signal_socket irq_socket;
    
    void b_transact(gs::gp::GenericSlaveAccessHandle ah) {};

private:
    /*
     * Parameter for the irq line.
     */
    gs::gs_param<uint32_t> irqNumber;
    gs::gs_param<uint32_t> m_size;

    Gsgp2cosim * m_gsgp2cosim;
    irq2gss * m_irq2gss;

    void end_of_elaboration(void);
};

#endif /* !NVDLA_COSIM_SC_WRAPPER_H */
