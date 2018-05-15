// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: irq2gss.h

#ifndef _IRQ2GSS_H_
#define _IRQ2GSS_H_

#ifndef SC_INCLUDE_DYNAMIC_PROCESSES
#define SC_INCLUDE_DYNAMIC_PROCESSES
#endif

#include <systemc.h>

#include "greensignalsocket/green_signal.h"
#include "shm_lib/simdb.hpp"
#include "shm_lib/shm_defs.h"
#include "SimpleCPU/IRQ.h"

class irq2gss : 
    public sc_core::sc_module
{
public:

    irq2gss( sc_core::sc_module_name name , uint32_t irq, const char* shm_name);
    ~irq2gss(void);

    SC_HAS_PROCESS(irq2gss);

    gs_generic_signal::initiator_signal_socket m_socket;

    void transport(void);

private:
    gs_generic_signal::gs_generic_signal_payload payload;
    IRQ_ext_data data;
    uint32_t irq_number;
    simdb shm_db;
};

#endif
