#ifndef _SHM_DEFS_H__
#define _SHM_DEFS_H__

#include <stdio.h>
#include <vector>
#define SIMDB_BLOCK_SIZE  1024
#define SIMDB_BLOCK_COUNT 1024

enum PAYLOAD_TYPE
{
    PAYLOAD_TYPE_APB = 0,
    PAYLOAD_TYPE_RAM
};

struct trans_payload
{
    unsigned int data;
    unsigned long long addr;
};

struct irq_trans_payload
{
    unsigned int value;
};

#endif