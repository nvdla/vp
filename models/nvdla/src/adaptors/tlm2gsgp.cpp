// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: tlm2gsgp.cpp

#include "adaptors/tlm2gsgp.h"
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

Tlm2gsgp::Tlm2gsgp(sc_core::sc_module_name name)
    : sc_module(name)
    , gsgp_master_port("gsgp_master_port")
    , tlm_slave_port("tlm_slave_port")
    , is_dmi(false)
{
    tlm_slave_port.register_b_transport(this, &Tlm2gsgp::b_transport);
    gsgp_master_port.out_port(*this);
}

Tlm2gsgp::~Tlm2gsgp()
{
}

void Tlm2gsgp::set_dmi_mutex(pthread_mutex_t *mtx)
{
    dmi_mtx = mtx;
    is_dmi = true;
}

void Tlm2gsgp::end_of_elaboration()
{
    // Create transaction
    transaction = gsgp_master_port.create_transaction();
}

void Tlm2gsgp::b_transport(tlm_generic_payload& tlm_gp, sc_time& delay)
{
    unsigned char *value = tlm_gp.get_data_ptr();
    uint64_t size = tlm_gp.get_data_length();
    gs::GSDataType::dtype data = gs::GSDataType::dtype(value, size);

    // Fill the transaction
    this->transaction->setMBurstLength(size);
    this->transaction->setMAddr(tlm_gp.get_address());
    this->transaction->setMData(data);
    this->transaction->setMCmd(tlm_gp.is_write()?gs::Generic_MCMD_WR:gs::Generic_MCMD_RD);
    this->transaction->get_tlm_transaction()->set_byte_enable_ptr(tlm_gp.get_byte_enable_ptr());
    this->transaction->get_tlm_transaction()->set_byte_enable_length(tlm_gp.get_byte_enable_length());

    if (is_dmi)
        pthread_mutex_lock(dmi_mtx);

    // Issue b_transport
    gsgp_master_port.Transact(this->transaction);

    if (is_dmi)
        pthread_mutex_unlock(dmi_mtx);
        
    // Send response
    tlm_gp.set_response_status(transaction->get_tlm_transaction()->get_response_status());
    
    if (tlm_gp.get_response_status() != TLM_OK_RESPONSE) {
        SC_REPORT_ERROR(name(), gp2str(tlm_gp).c_str());
    }

#if DEBUG_LOG
    SC_REPORT_INFO_VERB(name(), gp2str(tlm_gp).c_str(), verb);
#endif
}
