// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: thread_safe_event.cpp

#include "transactors/thread_safe_event.h"

void thread_safe_event::notify(sc_core::sc_time delay)
{
    m_delay = delay;
    async_request_update();
}

const sc_core::sc_event& thread_safe_event::default_event(void) const
{
    return m_event;
}

void thread_safe_event::update(void)
{
    m_event.notify(m_delay);
}
