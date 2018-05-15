// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: msi2gss.cpp

#include "transactors/msi2gss.h"
#include <poll.h>
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

void* PollMsi(void *arg)
{
    thread_state_t *thread_arg = reinterpret_cast<thread_state_t *>(arg);

    struct pollfd fds;
    fds.fd = thread_arg->fd;
    fds.events = POLLIN;
    
    int rc;
    while (1) {
        rc = poll(&fds, 1, 1000);
        if (rc < 0) {
            std::cerr << "ERROR when polling msi event, errno:" << errno << std::endl;
            assert(0);
        } else if (rc > 0) {
            if (fds.revents == POLLIN) {
                thread_arg->msi_event->notify();
            } else {
                std::cerr << "ERROR when polling msi event, revents=" << fds.revents << std::endl;
                assert(0);
            }
        }
        if (thread_arg->stop) {
#ifdef DEBUG_LOG
            std::cout << "Exit msi polling thread!" << std::endl;
#endif
            break;
        }
    }

    return NULL;
}

Msi2gss::Msi2gss( sc_core::sc_module_name name , uint32_t irq, pci_bar_handle_t pci_bar_handle_, int msi_fd, uint32_t msi_index_)
    : m_socket("m_socket")
    , irq_number(irq)
    , pci_bar_handle(pci_bar_handle_)
    , msi_event("msi_event")
    , msi_index(msi_index_)
{
    payload.set_data_ptr( reinterpret_cast<unsigned char*>( &data )  );

    gs::socket::config<gs_generic_signal::gs_generic_signal_protocol_types> cnf;
    cnf.use_mandatory_extension<IRQ_LINE_EXTENSION>();
    m_socket.set_config(cnf);

    /* Create thread to poll msi interrupt event */
    thread_state.fd = msi_fd;
    thread_state.stop = false;
    thread_state.msi_event = &msi_event;
    thread_state.thread = pthread_create(&(thread_state.thread), NULL, PollMsi, &thread_state);

    SC_THREAD( transport );
}

void Msi2gss::transport(void) 
{
    sc_time delay = SC_ZERO_TIME;
    data.irq_line = irq_number;
    uint32_t status;
    while(1) {
        /* Wait for interrupt event */
        wait(msi_event.default_event());
        /* Read back interrupt status */
        fpga_pci_peek(pci_bar_handle, CFG_INT_STATUS_ADDR, &status);
        data.value    = status & (0x1 << msi_index);
        /* Ack interrupt */
        fpga_pci_poke(pci_bar_handle, CFG_INT_STATUS_ADDR, (0x1 << msi_index));
        /* Transmit interrupt */
        m_socket.validate_extension<IRQ_LINE_EXTENSION>(payload);
        m_socket->b_transport( payload, delay );
#if DEBUG_LOG
        std::ostringstream oss;
        oss << "IRQ: irq_line=" << data.irq_line << " value=" << data.value;
        SC_REPORT_INFO_VERB(name(), oss.str().c_str(), verb);
#endif
    }
}

Msi2gss::~Msi2gss(void)
{
    thread_state.stop = 1;
    pthread_join(thread_state.thread, NULL);
}
