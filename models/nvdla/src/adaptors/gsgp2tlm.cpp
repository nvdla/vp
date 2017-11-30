// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: gsgp2tlm.cpp

#include "adaptors/gsgp2tlm.h"
#include "adaptors/gp2str.h"

using namespace std;
using namespace tlm;
using namespace sc_core;

#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif

#if DEBUG_LOG
static int const verb = SC_HIGH;
#endif

Gsgp2tlm::Gsgp2tlm(sc_core::sc_module_name name)
    : sc_module(name)
    , gsgp_slave_port("gsgp_slave_port")
    , tlm_master_port("tlm_master_port")
{
    this->gsgp_slave_port.bind_b_if(*this);
}

Gsgp2tlm::~Gsgp2tlm()
{
}

void Gsgp2tlm::end_of_elaboration()
{
}

void Gsgp2tlm::b_transact(gs::gp::GenericSlaveAccessHandle ah)
{
    // Fix delay to 0
    sc_time delay = SC_ZERO_TIME;

    // Get slave access handle
    gs::gp::GenericSlavePort<32>::accessHandle t = _getSlaveAccessHandle(ah);
    
    // Get data and size
    gs::GSDataType data;
    data.set(t->getMData());
    
    // Fill the transaction
    m_tlm_gp.set_command(t->getMCmd()==gs::Generic_MCMD_WR?TLM_WRITE_COMMAND:TLM_READ_COMMAND);
    m_tlm_gp.set_address(t->getMAddr() - gsgp_slave_port.base_addr);
    m_tlm_gp.set_data_ptr(data.getDataPtr()->getData());
    m_tlm_gp.set_data_length(data.getSize());
    m_tlm_gp.set_byte_enable_length(0);
    m_tlm_gp.set_response_status(TLM_INCOMPLETE_RESPONSE);

    // Issue b_transport
    tlm_master_port->b_transport(m_tlm_gp, delay);
    
    // Send response
    t->get_tlm_transaction()->set_response_status(m_tlm_gp.get_response_status());
    
    if (m_tlm_gp.get_response_status() != TLM_OK_RESPONSE) {
        SC_REPORT_ERROR(name(), gp2str(m_tlm_gp).c_str());
    }

#if DEBUG_LOG
    SC_REPORT_INFO_VERB(name(), gp2str(m_tlm_gp).c_str(), verb);
#endif
}
