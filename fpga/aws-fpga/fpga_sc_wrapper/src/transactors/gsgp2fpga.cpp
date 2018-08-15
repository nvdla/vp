// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: gsgp2fpga.cpp

#include "transactors/gsgp2fpga.h"

using namespace std;
using namespace sc_core;

#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif

#if DEBUG_LOG
static int const verb = SC_HIGH;
#endif

Gsgp2fpga::Gsgp2fpga(sc_core::sc_module_name name, pci_bar_handle_t pci_bar_handle_, uint64_t addr_offset_)
    : sc_module(name)
    , gsgp_slave_port("gsgp_slave_port")
    , pci_bar_handle(pci_bar_handle_)
    , addr_offset(addr_offset_)
{
    this->gsgp_slave_port.bind_b_if(*this);
}

Gsgp2fpga::~Gsgp2fpga()
{
}

void Gsgp2fpga::end_of_elaboration()
{
}

/* write data to FPGA RAM */
bool Gsgp2fpga::data_write(uint64_t addr, uint8_t *p_data, int len)
{
    bool ret  = true;
    int index = 0;
    bool poke_ret = false;

    if (len >= 4 && (len%4 != 0 || ((addr&0x3) != 0)))
        SC_REPORT_ERROR(name(), "ERROR: data_write only support 4 byte alignment read, when len is more than 4!\n");

    //Process the 4 byte alignment data.
    while (len >= 4)
    {
        poke_ret = fpga_pci_poke(pci_bar_handle, (addr+index), *(reinterpret_cast<uint32_t *>(&(p_data[index]))));
        ret   = ret & poke_ret;
        len   = len - 4;
        index = index + 4;
    }

    //Process the 1 byte data, need aws-fpga sdk v1.4 to support 8 bit read and write.
    if (len == 1)
    {
        poke_ret = fpga_pci_poke8(pci_bar_handle, (addr+index), *(reinterpret_cast<uint8_t *>(&(p_data[index]))));
        ret   = ret & poke_ret;
    }

    return ret;
}

/* read data from FPGA RAM */
bool Gsgp2fpga::data_read(uint64_t addr, uint8_t *p_data, int len)
{
    bool ret  = true;
    int index = 0;
    bool peek_ret = false;

    if (len >= 4 && (len%4 != 0 || ((addr&0x3) != 0)))
        SC_REPORT_ERROR(name(), "ERROR: data_read only support 4 byte alignment read, when len is more than 4!\n");
    
    while (len >= 4)
    {
        peek_ret = fpga_pci_peek(pci_bar_handle, (addr+index)&0xfffffffc, reinterpret_cast<uint32_t *>(&(p_data[index])));
        ret   = ret & peek_ret;
        len   = len - 4;
        index = index + 4;
    }

    if (len == 1)
    {
        peek_ret = fpga_pci_peek8(pci_bar_handle, (addr+index), reinterpret_cast<uint8_t *>(&(p_data[index])));
        ret   = ret & peek_ret;
    }

    return ret;
}

void Gsgp2fpga::b_transact(gs::gp::GenericSlaveAccessHandle ah)
{
    // Get slave access handle
    gs::gp::GenericSlavePort<32>::accessHandle t = _getSlaveAccessHandle(ah);
    
    // Get address, data
    gs::GSDataType data;
    data.set(t->getMData());

    if (data.getSize() == 0)
    {
#if DEBUG_LOG
        std::ostringstream oss;
        oss << "GP: data size is 0" << std::endl;
        SC_REPORT_INFO_VERB(name(), oss.str().c_str(), verb);
#endif
        return;
    }
    // Issue request
    uint64_t address = t->getMAddr() - gsgp_slave_port.base_addr + addr_offset;
    if (t->getMCmd() == gs::Generic_MCMD_WR) {
        if (0 != data_write(address, (reinterpret_cast<uint8_t *>(&(t->getMData()[0]))), static_cast<int>(data.getSize()))) {
            SC_REPORT_ERROR(name(), "ERROR on data write!\n");
        } else {
#if DEBUG_LOG
            std::ostringstream oss;
            oss << "GP: iswrite=1 addr=0x" << std::hex << address << std::dec << " len=" << data.getSize() << " data=0x " << std::hex << *(reinterpret_cast<uint32_t *>(&(t->getMData()[0]))) << std::endl;
            SC_REPORT_INFO_VERB(name(), oss.str().c_str(), verb);
#endif
        }
    } else {
        if (0 != data_read(address, reinterpret_cast<uint8_t *>(&(t->getMData()[0])),static_cast<int>(data.getSize()))) {
            SC_REPORT_ERROR(name(), "ERROR on data read!\n");
        } else {
#if DEBUG_LOG
            std::ostringstream oss;
            oss << "GP: iswrite=0 addr=0x" << std::hex << address << std::dec << " len=" << data.getSize() << " data=0x " << std::hex << *(reinterpret_cast<uint32_t *>(&(t->getMData()[0]))) << std::endl;
            SC_REPORT_INFO_VERB(name(), oss.str().c_str(), verb);
#endif
        }
    }
}

void Gsgp2fpga::set_addr_offset(uint64_t addr)
{
    addr_offset = addr;
    std::cout << "Set Offset Address: " << hex << addr_offset <<endl;
}
