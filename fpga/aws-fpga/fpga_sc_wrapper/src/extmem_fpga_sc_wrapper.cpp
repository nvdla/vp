// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: extmem_fpga_sc_wrapper.cpp

#include "fpga_sc_wrapper/extmem_fpga_sc_wrapper.h"
#include "transactors/gsgp2fpga.h"
#include "transactors/msi2gss.h"

using namespace sc_core;

extmem_fpga_sc_wrapper::extmem_fpga_sc_wrapper(sc_module_name name, pci_bar_handle_t pci_bar_handle):
    target_port("target_port"),
    m_size("size", 0x100)
{
    // Create adaptors
    m_gsgp2fpga = new Gsgp2fpga("extmem_adaptor", pci_bar_handle, 0xC0000000);
    
    // Bind CSB port
    this->target_port.bind( m_gsgp2fpga->gsgp_slave_port );
    m_gsgp2fpga->set_addr_offset(this->target_port.base_addr);
}

/**
 * Destructor
 */
extmem_fpga_sc_wrapper::~extmem_fpga_sc_wrapper()
{
    if (m_gsgp2fpga)
        delete m_gsgp2fpga;
}

/*
 *
 * End of elaboration
 */
void extmem_fpga_sc_wrapper::end_of_elaboration()
{
}
