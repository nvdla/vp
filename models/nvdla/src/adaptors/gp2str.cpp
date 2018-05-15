// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: gp2str.cpp

#include "adaptors/gp2str.h"
#include <sstream>
#include <iomanip>

using namespace std;

std::string gp2str(const tlm::tlm_generic_payload &gp)
{
  uint32_t const bytes_in_word = sizeof(uint32_t);

  uint64_t  address     = gp.get_address();
  uint8_t*  data_ptr    = gp.get_data_ptr();
  uint32_t  data_len    = gp.get_data_length();
  uint8_t*  be_ptr      = gp.get_byte_enable_ptr();
  uint32_t  be_len      = gp.get_byte_enable_length();

  if (be_ptr && be_len == 0) return "ERROR GENERIC PAYLOAD: byte_enable_length == 0!";
  if (be_ptr == NULL && be_len>0) return "ERROR GENERIC PAYLOAD: byte_enable_ptr == NULL!";
  if (data_ptr == NULL && data_len>0) return "ERROR GENERIC PAYLOAD: data_ptr == NULL!";

  std::ostringstream data_oss;
  data_oss << "0x";

  uint32_t remain_len = data_len;
  uint32_t idx = 0;

  if (address % bytes_in_word != 0) {
    data_oss << " ";
    uint32_t bytes = (bytes_in_word - address%bytes_in_word) > remain_len ?
                remain_len : (bytes_in_word- address%bytes_in_word);
    for (uint32_t i=0; i<bytes; i++) {
      data_oss << getByte(data_ptr+idx, getBe(be_ptr, be_len, idx), !i );
      idx++;
      remain_len--;
    }
  }

  while (remain_len > 0) {
    data_oss << " ";
    if ( remain_len >= bytes_in_word) {
      bool word_enable = true;
      for (uint32_t i=0;i<bytes_in_word;i++) word_enable &= getBe(be_ptr, be_len, idx+i);
      if (word_enable) {
        data_oss << std::hex << std::setw(8) << std::setfill('0') << *reinterpret_cast<const uint32_t*>(data_ptr+idx);
        idx += bytes_in_word;
      } else {
        for (uint32_t i=0;i<bytes_in_word;i++) {
          data_oss << getByte(data_ptr+idx, getBe(be_ptr, be_len, idx), !i );
          idx++;
        }
      }
      remain_len -= bytes_in_word;
    } else { // remain_len < bytes_in_word
      for (uint32_t i=0; i<remain_len; i++) {
        data_oss << getByte(data_ptr+idx, getBe(be_ptr, be_len, idx), !i );
        idx++;
      }
      remain_len = 0;
    }
  }

  std::ostringstream oss;
  oss << "GP: iswrite=" << gp.is_write() << " addr=0x" << std::hex << address
      << std::dec << " len=" << data_len << " data=" << data_oss.str() << " resp=" << gp.get_response_string();

  return oss.str();
}

inline bool getBe(uint8_t* ptr, uint32_t len, uint32_t idx)
{
  if (ptr == NULL) return true;
  assert(len);
  return (ptr[idx % len] == TLM_BYTE_ENABLED);
}

inline std::string getByte(uint8_t *ptr, bool valid, bool is_first_byte_of_word)
{
  std::string s;
  if (!is_first_byte_of_word) s = "_";

  if (!valid) {
    s += "XX";
  } else {
    char c[4];
    sprintf( c, "%02x", *ptr);
    s += c;
  }
  return s;
}

