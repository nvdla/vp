// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: irq2gss.cpp

#include "transactors/irq2gss.h"
#include <errno.h>

using namespace sc_core;

#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif

#if DEBUG_LOG
#include <sstream>
static int const verb = SC_HIGH;
#endif

#define CFG_INT_STATUS_ADDR (0x0)

irq2gss::irq2gss( sc_core::sc_module_name name , uint32_t irq, const char* shm_name)
    : m_socket("m_socket")
    , irq_number(irq)
    , shm_db(shm_name, SIMDB_BLOCK_SIZE, SIMDB_BLOCK_COUNT)
{
    payload.set_data_ptr( reinterpret_cast<unsigned char*>( &data )  );

    gs::socket::config<gs_generic_signal::gs_generic_signal_protocol_types> cnf;
    cnf.use_mandatory_extension<IRQ_LINE_EXTENSION>();
    m_socket.set_config(cnf);

    auto keys = shm_db.getKeyStrs();
    for(unsigned int i = 0; i < keys.size(); i++) {
        shm_db.del(keys[i].str);
    }

    SC_THREAD( transport );
}

void irq2gss::transport(void) 
{
    sc_time delay = SC_ZERO_TIME;
    data.irq_line = irq_number;
    uint32_t status;

    while(1)
    {
        wait(1, SC_MS);
        auto tpv_up = shm_db.get<struct irq_trans_payload>("irq_ipc_db_up");
        auto tpv_dn = shm_db.get<struct irq_trans_payload>("irq_ipc_db_dn");

        if (!tpv_up.empty())
        {
            if (!tpv_dn.empty())
            {
                cout << name() << " ERROR: The last INT down stream FIFO is not empty!\n";
                shm_db.del("irq_ipc_db_dn");
            }

            status = tpv_up[0].value;
            data.value = status & (0x1 << 0x0);

            std::vector<struct irq_trans_payload> v;
            struct irq_trans_payload t;
            t.value = tpv_up[0].value;
            v.push_back(t);

            shm_db.del("irq_ipc_db_up");

            // clear the INT
            shm_db.put<struct irq_trans_payload>("irq_ipc_db_dn", v);

            while(1)
            {
                wait(1, SC_NS);
                tpv_dn = shm_db.get<struct irq_trans_payload>("irq_ipc_db_dn");
                if (tpv_dn.empty())
                    break;
            }

            /* Transmit interrupt */
            m_socket.validate_extension<IRQ_LINE_EXTENSION>(payload);
            m_socket->b_transport( payload, delay );
        }
    }
}

irq2gss::~irq2gss(void)
{}
