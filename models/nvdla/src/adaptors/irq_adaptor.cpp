// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: IrqAdaptor.h

#include "adaptors/irq_adaptor.h"

using namespace tlm;
using namespace sc_core;

#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif

#if DEBUG_LOG
#include <sstream>
static int const verb = SC_HIGH;
#endif

IrqAdaptor::IrqAdaptor( sc_core::sc_module_name name , uint32_t irq)
    : m_socket("m_socket"),
      irq_number(irq)
{
    payload.set_data_ptr( reinterpret_cast<unsigned char*>( &data )  );

    gs::socket::config<gs_generic_signal::gs_generic_signal_protocol_types> cnf;
    cnf.use_mandatory_extension<IRQ_LINE_EXTENSION>();
    m_socket.set_config(cnf);

    SC_THREAD( transport );
    sensitive << m_signal;
}

void IrqAdaptor::transport(void) 
{
    sc_time delay = SC_ZERO_TIME;
    while(1) {
        wait();
        data.value    = m_signal.read();
        data.irq_line = irq_number;
        m_socket.validate_extension<IRQ_LINE_EXTENSION>(payload);
        m_socket->b_transport( payload, delay );
#if DEBUG_LOG
        std::ostringstream oss;
        oss << "IRQ: irq_line=" << data.irq_line << " value=" << data.value;
        SC_REPORT_INFO_VERB(name(), oss.str().c_str(), verb);
#endif
    }
}
