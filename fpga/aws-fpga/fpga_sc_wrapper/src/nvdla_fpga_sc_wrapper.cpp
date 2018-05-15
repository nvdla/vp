// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: nvdla_fpga_sc_wrapper.cpp

#include "fpga_sc_wrapper/nvdla_fpga_sc_wrapper.h"
#include "transactors/gsgp2fpga.h"
#include "transactors/msi2gss.h"
#include "SimpleCPU/IRQ.h"

using namespace sc_core;

nvdla_fpga_sc_wrapper::nvdla_fpga_sc_wrapper(sc_module_name name, pci_bar_handle_t pci_bar_handle_csb, pci_bar_handle_t pci_bar_handle_cfg, int msi_fd):
    target_port("csb_port"),
    irq_socket("irq_socket"),
    irqNumber("irq_number", 0),
    m_size("size", 0x100)
{
    gs::socket::config<gs_generic_signal::gs_generic_signal_protocol_types> cnf;
    cnf.use_mandatory_extension<IRQ_LINE_EXTENSION>();
    irq_socket.set_config(cnf);

    // Create adaptors
    m_gsgp2fpga = new Gsgp2fpga("csb_adaptor", pci_bar_handle_csb);
    m_msi2gss = new Msi2gss("irq_adaptor", (uint32_t)irqNumber, pci_bar_handle_cfg, msi_fd, 0x0/*msi_index*/);
    
    // Bind CSB port
    this->target_port.bind( m_gsgp2fpga->gsgp_slave_port );
    
    // Bind IRQ port
    m_msi2gss->m_socket.bind( irq_socket );
}

/**
 * Destructor
 */
nvdla_fpga_sc_wrapper::~nvdla_fpga_sc_wrapper()
{
    if (m_gsgp2fpga)
        delete m_gsgp2fpga;
    if (m_msi2gss)
        delete m_msi2gss;
}

/*
 *
 * End of elaboration
 */
void nvdla_fpga_sc_wrapper::end_of_elaboration()
{
}
