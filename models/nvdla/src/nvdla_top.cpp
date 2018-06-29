// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: nvdla_top.cpp

#include "nvdla/nvdla_top.h"
#include "nvdla/IRQ.h"

#include "adaptors/gsgp2tlm.h"
#include "adaptors/tlm2gsgp.h"
#include "adaptors/irq_adaptor.h"

#include "NV_nvdla.h"

USING_SCSIM_NAMESPACE(cmod)

using namespace sc_core;
using namespace tlm;

/**
 * NVDLA SystemC module
 *
 * @param name Instance name
 * @param irq_number IRQ number
 * @param
 */
NVDLA_top::NVDLA_top(sc_module_name name):
    target_port("csb_port"),
    m_mc_port("m_mc_port"),
    m_cv_port("m_cv_port"),
    irq_socket("irq_socket"),
    irqNumber("irq_number", 0),
    m_size("size", 0x100)
{
    gs::socket::config<gs_generic_signal::gs_generic_signal_protocol_types> cnf;
    cnf.use_mandatory_extension<IRQ_LINE_EXTENSION>();
    irq_socket.set_config(cnf);

    // Create nvdla and adaptors
    m_nvdla = new NV_nvdla("nvdla_core");
    m_gsgp2tlm = new Gsgp2tlm("csb_adaptor");
    m_tlm2gsgp_mc = new Tlm2gsgp("dbb_adaptor");
    m_tlm2gsgp_cv = new Tlm2gsgp("sram_adaptor");
    m_irq_adaptor = new IrqAdaptor("irq_adaptor", (uint32_t)irqNumber);
    
    // Bind CSB port
    this->target_port.bind( m_gsgp2tlm->gsgp_slave_port );
    m_gsgp2tlm->tlm_master_port.bind( m_nvdla->nvdla_host_master_if );
    
    // Bind Memory port
    m_nvdla->nvdla_core2dbb_axi4.bind(m_tlm2gsgp_mc->tlm_slave_port);
    m_nvdla->nvdla_core2cvsram_axi4.bind(m_tlm2gsgp_cv->tlm_slave_port);
    m_tlm2gsgp_mc->gsgp_master_port.bind(this->m_mc_port);
    m_tlm2gsgp_cv->gsgp_master_port.bind(this->m_cv_port);
    this->m_mc_port.out_port(*this);
    this->m_cv_port.out_port(*this);

    // Bind IRQ port
    m_nvdla->nvdla_intr( m_coreIrq );
    m_irq_adaptor->m_signal( m_coreIrq );
    m_irq_adaptor->m_socket.bind( irq_socket );
}

/**
 * Destructor
 */
NVDLA_top::~NVDLA_top()
{
    if (m_nvdla)
        delete m_nvdla;
    if (m_gsgp2tlm)
        delete m_gsgp2tlm;
    if (m_tlm2gsgp_mc)
        delete m_tlm2gsgp_mc;
    if (m_tlm2gsgp_cv)
        delete m_tlm2gsgp_cv;
    if (m_irq_adaptor)
        delete m_irq_adaptor;
}

void NVDLA_top::set_dmi_mutex(pthread_mutex_t *mtx)
{
    m_tlm2gsgp_mc->set_dmi_mutex(mtx);
}

/*
 *
 * End of elaboration
 */
void NVDLA_top::end_of_elaboration()
{
}
