// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: gsgp2tlm.h

#ifndef GSGP2TLM_H
#define GSGP2TLM_H

#ifndef SC_INCLUDE_DYNAMIC_PROCESSES
#define SC_INCLUDE_DYNAMIC_PROCESSES
#endif

#include <systemc.h>
#include <tlm.h>
#include "tlm_utils/multi_passthrough_initiator_socket.h"

#include "gsgpsocket/transport/GSGPSlaveSocket.h"

class Gsgp2tlm 
    : public sc_core::sc_module 
    , public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
{
  public:
    Gsgp2tlm(sc_core::sc_module_name name);
    ~Gsgp2tlm();
        
    SC_HAS_PROCESS(Gsgp2tlm);
        
    gs::gp::GenericSlavePort<32> gsgp_slave_port;
    void b_transact(gs::gp::GenericSlaveAccessHandle ah);
    
    tlm_utils::multi_passthrough_initiator_socket<Gsgp2tlm> tlm_master_port;
    
private:
    void end_of_elaboration();
        
    tlm::tlm_generic_payload                        m_tlm_gp;
};

#endif /* !GSGP2TLM_H */
