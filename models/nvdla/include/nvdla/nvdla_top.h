// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: nvdla_top.h

#ifndef NVDLA_TOP_H
#define NVDLA_TOP_H

#include <systemc.h>
#include <tlm.h>
#include <pthread.h>
#include "tlm_utils/multi_passthrough_initiator_socket.h"
#include "tlm_utils/multi_passthrough_target_socket.h" 

#include "greensignalsocket/green_signal.h"

#include "gsgpsocket/transport/GSGPSlaveSocket.h"
#include "gsgpsocket/transport/GSGPMasterBlockingSocket.h"

class IrqAdaptor;
class Gsgp2tlm;
class Tlm2gsgp;
// FIXME get rid of it...
namespace scsim {
    namespace cmod {
        class NV_nvdla;
    }
}

class NVDLA_top 
    : public sc_module
    , public gs::tlm_b_if<gs::gp::GenericSlaveAccessHandle>
    , public gs::payload_event_queue_output_if<gs::gp::master_atom>
{
public:
    NVDLA_top(sc_core::sc_module_name name);
    ~NVDLA_top();
    
    typedef gs_generic_signal::gs_generic_signal_payload irqPayload;
    gs::gp::GenericSlavePort<32> target_port;
    gs::gp::GenericMasterBlockingPort<32> m_mc_port;
    gs::gp::GenericMasterBlockingPort<32> m_cv_port;
    gs_generic_signal::initiator_signal_socket irq_socket;
    
    void b_transact(gs::gp::GenericSlaveAccessHandle ah) {};

    void set_dmi_mutex(pthread_mutex_t *mtx);

private:
    /*
     * Parameter for the irq line.
     */
    gs::gs_param<uint32_t> irqNumber;
    gs::gs_param<uint32_t> m_size;

    IrqAdaptor *m_irq_adaptor;
    Gsgp2tlm * m_gsgp2tlm;
    Tlm2gsgp * m_tlm2gsgp_mc;
    Tlm2gsgp * m_tlm2gsgp_cv;

    scsim::cmod::NV_nvdla *m_nvdla;
    sc_buffer<bool> m_coreIrq;

    void notify(gs::gp::master_atom& tc) {};
    void end_of_elaboration(void);
};

#endif /* !NVDLA_TOP_H */
