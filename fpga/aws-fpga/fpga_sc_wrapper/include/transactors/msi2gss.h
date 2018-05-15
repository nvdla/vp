// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: msi2gss.h

#ifndef _MSI2GSS_H_
#define _MSI2GSS_H_

#ifndef SC_INCLUDE_DYNAMIC_PROCESSES
#define SC_INCLUDE_DYNAMIC_PROCESSES
#endif

#include <systemc.h>

#include <pthread.h>
#include "greensignalsocket/green_signal.h"
#include "fpga_pci.h"
#include "SimpleCPU/IRQ.h"
#include "transactors/thread_safe_event.h"

typedef struct {
    int fd;
    thread_safe_event *msi_event;
    bool stop;
    pthread_t thread;
} thread_state_t;

class Msi2gss : 
    public sc_core::sc_module
{
public:
    Msi2gss( sc_core::sc_module_name name , uint32_t irq, pci_bar_handle_t pci_bar_handle, int msi_fd, uint32_t msi_index);
    ~Msi2gss(void);

    SC_HAS_PROCESS(Msi2gss);

    gs_generic_signal::initiator_signal_socket m_socket;

    void transport(void);

private:
    gs_generic_signal::gs_generic_signal_payload payload;
    IRQ_ext_data data;
    uint32_t irq_number;
    pci_bar_handle_t pci_bar_handle;
    thread_safe_event msi_event;
    uint32_t msi_index;
    thread_state_t thread_state;
};

#endif
