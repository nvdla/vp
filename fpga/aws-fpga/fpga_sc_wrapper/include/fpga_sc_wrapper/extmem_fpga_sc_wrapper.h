// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: extmem_fpga_sc_wrapper.h

#ifndef EXTMEM_FPGA_SC_WRAPPER_H
#define EXTMEM_FPGA_SC_WRAPPER_H

#include <systemc.h>
#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "fpga_pci.h"

class Gsgp2fpga;

class extmem_fpga_sc_wrapper 
    : public sc_module
    , public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
{
public:
    extmem_fpga_sc_wrapper(sc_core::sc_module_name name, pci_bar_handle_t pci_bar_handle);
    ~extmem_fpga_sc_wrapper();
    
    gs::gp::GenericSlavePort<32> target_port;
    
    void b_transact(gs::gp::GenericSlaveAccessHandle ah) {};

private:
    gs::gs_param<uint32_t> m_size;

    Gsgp2fpga * m_gsgp2fpga;

    void end_of_elaboration(void);
};

#endif /* !EXTMEM_FPGA_SC_WRAPPER_H */
