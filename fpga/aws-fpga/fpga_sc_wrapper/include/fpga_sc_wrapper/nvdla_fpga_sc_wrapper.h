// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: nvdla_fpga_sc_wrapper.h

#ifndef NVDLA_FPGA_SC_WRAPPER_H
#define NVDLA_FPGA_SC_WRAPPER_H

#include <systemc.h>
#include "greensignalsocket/green_signal.h"
#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "fpga_pci.h"

class Gsgp2fpga;
class Msi2gss;

class nvdla_fpga_sc_wrapper 
    : public sc_module
    , public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
{
public:
    nvdla_fpga_sc_wrapper(sc_core::sc_module_name name, pci_bar_handle_t pci_bar_handle_csb, pci_bar_handle_t pci_bar_handle_cfg, int msi_fd);
    ~nvdla_fpga_sc_wrapper();
    
    gs::gp::GenericSlavePort<32> target_port;
    gs_generic_signal::initiator_signal_socket irq_socket;
    
    void b_transact(gs::gp::GenericSlaveAccessHandle ah) {};

private:
    /*
     * Parameter for the irq line.
     */
    gs::gs_param<uint32_t> irqNumber;
    gs::gs_param<uint32_t> m_size;

    Gsgp2fpga * m_gsgp2fpga;
    Msi2gss * m_msi2gss;

    void end_of_elaboration(void);
};

#endif /* !NVDLA_FPGA_SC_WRAPPER_H */
