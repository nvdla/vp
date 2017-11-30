// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: irq_adaptor.h

#ifndef _IRQ_ADAPTOR_H_
#define _IRQ_ADAPTOR_H_

#ifndef SC_INCLUDE_DYNAMIC_PROCESSES
#define SC_INCLUDE_DYNAMIC_PROCESSES
#endif

#include <systemc.h>

#include "greensignalsocket/green_signal.h"

#include "nvdla/IRQ.h"

class IrqAdaptor : 
    public sc_core::sc_module
{
public:
    IrqAdaptor( sc_core::sc_module_name name , uint32_t irq);
    SC_HAS_PROCESS(IrqAdaptor);

    sc_core::sc_in< bool > m_signal;
    gs_generic_signal::initiator_signal_socket m_socket;

    void transport(void);

private:
    gs_generic_signal::gs_generic_signal_payload payload;
    sc_core::sc_time m_delay;
    IRQ_ext_data data;
    uint32_t irq_number;
    bool ack_requirement;
};

#endif
