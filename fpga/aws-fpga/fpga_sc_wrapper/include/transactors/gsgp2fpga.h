// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: gsgp2fpga.h

#ifndef GSGP2FPGA_H
#define GSGP2FPGA_H

#ifndef SC_INCLUDE_DYNAMIC_PROCESSES
#define SC_INCLUDE_DYNAMIC_PROCESSES
#endif

#include <systemc.h>
#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "fpga_pci.h"

class Gsgp2fpga
    : public sc_core::sc_module 
    , public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
{
  public:
    Gsgp2fpga(sc_core::sc_module_name name, pci_bar_handle_t pci_bar_handle, uint64_t addr_offset=0);
    ~Gsgp2fpga();
        
    SC_HAS_PROCESS(Gsgp2fpga);
        
    gs::gp::GenericSlavePort<32> gsgp_slave_port;
    void b_transact(gs::gp::GenericSlaveAccessHandle ah);
    void set_addr_offset(uint64_t addr);
    
private:
    void end_of_elaboration();
    pci_bar_handle_t pci_bar_handle;
    uint64_t addr_offset;

    bool data_write(uint64_t addr, uint8_t *p_data, int len);
    bool data_read(uint64_t addr, uint8_t *p_data, int len);

};

#endif
