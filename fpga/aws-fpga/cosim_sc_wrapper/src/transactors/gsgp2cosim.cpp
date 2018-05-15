// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: gsgp2cosim.cpp

#include "transactors/gsgp2cosim.h"

using namespace std;
using namespace sc_core;

#ifndef DEBUG_LOG
#define DEBUG_LOG 0
#endif

#if DEBUG_LOG
static int const verb = SC_HIGH;
#endif

Gsgp2cosim::Gsgp2cosim(sc_core::sc_module_name name, const char* shm_name, uint64_t addr_offset_)
    : sc_module(name)
    , gsgp_slave_port("gsgp_slave_port")
    , shm_db(shm_name, SIMDB_BLOCK_SIZE, SIMDB_BLOCK_COUNT)
    , shm_name_str(shm_name)
    , addr_offset(addr_offset_)
{
    this->gsgp_slave_port.bind_b_if(*this);

    auto keys = shm_db.getKeyStrs();
    for(unsigned int i = 0; i < keys.size(); i++) {
        shm_db.del(keys[i].str);
    }

    char *timeout_c;
    timeout_c = getenv("COSIM_SHM_TIMEOUT_IN_NS");
    if (!timeout_c)
    {
#if DEBUG_LOG
        cout << "COSIM_SHM_TIMEOUT_IN_NS is not set use default 0xFFFFFFFF" << endl;
#endif
        shm_timeout = 0xFFFFFFFF;
    } else {
#if DEBUG_LOG
        cout << "COSIM_SHM_TIMEOUT_IN_NS is " << timeout_c << endl;
#endif
        shm_timeout = strtoul(timeout_c, NULL, 0);
    }
}

Gsgp2cosim::~Gsgp2cosim()
{
}

void Gsgp2cosim::end_of_elaboration()
{
}

void Gsgp2cosim::b_transact(gs::gp::GenericSlaveAccessHandle ah)
{
    // Get slave access handle
    gs::gp::GenericSlavePort<32>::accessHandle t = _getSlaveAccessHandle(ah);
    
    // Get address, data, size must be 4
    gs::GSDataType data;
    data.set(t->getMData());
    sc_assert(data.getSize() == 4);

    struct trans_payload tp;

    std::stringstream str_wr_up;
    std::stringstream str_wr_dn;
    std::stringstream str_rd_up;
    std::stringstream str_rd_dn;
    str_wr_up << shm_name_str << "_wr_up";
    str_wr_dn << shm_name_str << "_wr_dn";
    str_rd_up << shm_name_str << "_rd_up";
    str_rd_dn << shm_name_str << "_rd_dn";

    uint32_t timeout;

    // Issue request
    uint64_t address = t->getMAddr() - gsgp_slave_port.base_addr + addr_offset;

    if (t->getMCmd() == gs::Generic_MCMD_WR) {
        auto tpv_dn = shm_db.get<struct trans_payload>(str_wr_dn.str());
        auto tpv_up = shm_db.get<struct trans_payload>(str_wr_up.str());
        if (!tpv_up.empty())
        {
            SC_REPORT_INFO(name(), "ERROR: The last write up stream FIFO is not empty!\n");
            shm_db.del(str_wr_up.str());
        }
        if (!tpv_dn.empty())
        {
            SC_REPORT_INFO(name(), "ERROR: The last write down stream is not done yet!\n");
            shm_db.del(str_wr_dn.str());
        }
        std::vector<struct trans_payload> v;
        tp.addr = address;
        tp.data = *(reinterpret_cast<uint32_t *>(&(t->getMData()[0])));
        v.push_back(tp);
        shm_db.put<struct trans_payload>(str_wr_dn.str(), v);

#if DEBUG_LOG
        uint32_t wr_ret = 0;
#endif
        timeout = shm_timeout;
        while(1) {
            tpv_up = shm_db.get<struct trans_payload>(str_wr_up.str());
            if(!tpv_up.empty()) {
#if DEBUG_LOG
                wr_ret = tp.data;
                std::ostringstream oss;
                oss << "Get write ack" << std::hex << wr_ret << std::endl;
                SC_REPORT_INFO_VERB(name(), oss.str().c_str(), verb);
#endif
                break;
            }
            wait(1000, SC_PS);
            if (timeout == 0)
            {
                SC_REPORT_INFO(name(), "Write timeout\n");
                break;
            } else {
                timeout--;
            }
        }
        shm_db.del(str_wr_up.str());
    } else {
        auto tpv_dn = shm_db.get<struct trans_payload>(str_rd_dn.str());
        auto tpv_up = shm_db.get<struct trans_payload>(str_rd_up.str());

        if (!tpv_up.empty())
        {
            SC_REPORT_INFO(name(), "ERROR: The last read up stream FIFO is not empty!\n");
            shm_db.del(str_rd_up.str());
        }
        if (!tpv_dn.empty())
        {
            SC_REPORT_INFO(name(), "ERROR: The last read down stream is not done yet!\n");
            shm_db.del(str_rd_dn.str());
        }

        std::vector<struct trans_payload> v;
        tp.addr = address;
        //tp.data = *(reinterpret_cast<uint32_t *>(&(t->getMData()[0])));
        v.push_back(tp);
        shm_db.put<struct trans_payload>(str_rd_dn.str(), v);

        timeout = shm_timeout;
        while(1) {
            tpv_up = shm_db.get<struct trans_payload>(str_rd_up.str());
            if(!tpv_up.empty()) {
                *(reinterpret_cast<uint32_t *>(&(t->getMData()[0]))) = tpv_up[0].data;
#if DEBUG_LOG
                std::ostringstream oss;
                oss << "Get read ack" << std::hex << tpv_up[0].data << std::endl;
                SC_REPORT_INFO_VERB(name(), oss.str().c_str(), verb);
#endif
                break;
            }
            wait(1000, SC_PS);
            if (timeout == 0)
            {
                SC_REPORT_INFO(name(), "Read timeout\n");
                break;
            } else {
                timeout--;
            }
        }

        shm_db.del(str_rd_up.str());
    }
}
