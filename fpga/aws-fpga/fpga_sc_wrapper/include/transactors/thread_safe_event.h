// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: thread_safe_event.h

#ifndef THREAD_SAFE_EVENT_H
#define THREAD_SAFE_EVENT_H

#include <systemc>

struct thread_safe_event_if: sc_core::sc_interface
{
    virtual void notify(sc_core::sc_time delay = sc_core::SC_ZERO_TIME) = 0;
    virtual const sc_core::sc_event& default_event(void) const = 0;

    protected:
    virtual void update(void) = 0;
};

struct thread_safe_event: sc_core::sc_prim_channel, thread_safe_event_if
{
    thread_safe_event(const char* name);
    void notify(sc_core::sc_time delay = sc_core::SC_ZERO_TIME);
    const sc_core::sc_event& default_event(void) const;

    protected:
    virtual void update(void);

    private:
    sc_core::sc_event m_event;
    sc_core::sc_time m_delay;
};

#endif
