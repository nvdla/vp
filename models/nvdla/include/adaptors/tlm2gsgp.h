// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: tlm2gsgp.h

#ifndef TLM2GSGP_H
#define TLM2GSGP_H

#ifndef SC_INCLUDE_DYNAMIC_PROCESSES
#define SC_INCLUDE_DYNAMIC_PROCESSES
#endif

#include <systemc.h>
#include <tlm.h>
#include <pthread.h>
#include "tlm_utils/simple_target_socket.h"

#include "gsgpsocket/transport/GSGPMasterBlockingSocket.h"

class Tlm2gsgp 
    : public sc_core::sc_module 
    , public gs::payload_event_queue_output_if<gs::gp::master_atom>
{
public:
    Tlm2gsgp(sc_core::sc_module_name name);
    ~Tlm2gsgp();
        
    SC_HAS_PROCESS(Tlm2gsgp);
    
    typedef gs::gp::GenericMasterBlockingPort<32>::accessHandle transactionHandle;
        
    gs::gp::GenericMasterBlockingPort<32> gsgp_master_port;
    
    tlm_utils::simple_target_socket<Tlm2gsgp> tlm_slave_port;

    void set_dmi_mutex(pthread_mutex_t *mtx);
    
private:
    void notify(gs::gp::master_atom& tc) {};
    void end_of_elaboration();
    void b_transport(tlm::tlm_generic_payload& tlm_gp, sc_time& delay);

    transactionHandle transaction;      /*<! Transaction to be posted. */

    pthread_mutex_t *dmi_mtx;

    bool is_dmi;
};

#endif /* !TLM2GSGP_H */
