// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: gp2str.h

#ifndef GP2STR_H
#define GP2STR_H

#include <systemc.h>
#include <tlm.h>

std::string gp2str(const tlm::tlm_generic_payload &gp);
inline bool getBe(uint8_t* ptr, uint32_t len, uint32_t idx);
inline std::string getByte(uint8_t *ptr, bool valid, bool is_first_byte_of_word);

#endif /* !GP2STR_H */
