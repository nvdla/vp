// ================================================================
// NVDLA Open Source Project
// 
// Copyright(c) 2016 - 2017 NVIDIA Corporation.  Licensed under the
// NVDLA Open Hardware License; Check "LICENSE" which comes with 
// this distribution for more information.
// ================================================================

// File Name: nvdla_bdma_mmio.c


#include <sys/mman.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <errno.h>

#include "arnvdla.uh"
#include "arnvdla.h"

#define LOW32(addr)   ((uint32_t)((addr) & 0xffffffff))
#define HIGH32(addr)  ((uint32_t)( ((addr)>>32) & 0xffffffff ))

/*
 * Platform Specific Params
 */
#define MEMORY_BASE 0xC0000000
#define MEMORY_SIZE (0x800 * 4)

#define NVDLA_MMIO_BASE  0x10200000
#define NVDLA_MMIO_SIZE  (0x10220000 - 0x10200000)

typedef enum {
    eDEVICE_MEM = 0,
    eDEVICE_DLA,
    eDEVICE_NUM
} nv_device;

void* mmap_device(nv_device dev)
{
    size_t size  = 0;
    off_t offset = 0;
    switch(dev) {
    case eDEVICE_DLA:
        size = NVDLA_MMIO_SIZE;
        offset = NVDLA_MMIO_BASE;
        break;
    case eDEVICE_MEM:
        size = MEMORY_SIZE;
        offset = MEMORY_BASE;
        break;
    default:
        printf("Unknown device\n");
        exit(EXIT_FAILURE);
    }

    int fd = open("/dev/mem", O_RDWR);
	if (fd < 0) {
		perror("open /dev/mem failed!");
        exit(EXIT_FAILURE);
	}

    void* device_mm = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, offset);
    if (close(fd) < 0) {
        perror("close failed!");
    }
    if (device_mm == MAP_FAILED) {
        perror("mmap DLA failed!");
        exit(EXIT_FAILURE);
    }
    return device_mm;
}


void munmap_device(nv_device dev, void* mm)
{
    size_t size  = 0;
    switch(dev) {
    case eDEVICE_DLA:
        size = NVDLA_MMIO_SIZE;
        break;
    case eDEVICE_MEM:
        size = MEMORY_SIZE;
        break;
    default:
        printf("Unknown device\n");
        exit(EXIT_FAILURE);
    }

    if (munmap(mm, size) < 0) {
        perror("mumap failed!");
    }
}


struct dla_reg {
    int32_t  offset;
    uint32_t value;
};


/* C x W x H */
#define TENSOR_MAX_DIMS  3

struct tensor {
    uint32_t  dims;
    uint32_t  size[TENSOR_MAX_DIMS];
    uint32_t  bits_per_element;

    /* Assume continuous memory */
    uint64_t  base;
};

/*
 *  Prepare data buffer
 */
void test_data(struct tensor* in, struct tensor* out, void* mem_mm)
{
    if (mem_mm == NULL) {
        printf("Invalid memory handle\n");
        exit(EXIT_FAILURE);
    }
    uint8_t* mm = (uint8_t*)mem_mm;

    in->dims = 2;
    in->size[0] = 0x100;
    in->size[1] = 8;
    in->bits_per_element = 1;
    in->base = MEMORY_BASE;

    int i = 0;
    uint32_t* mem = (uint32_t*)(mm + in->base - MEMORY_BASE);
    for (i = 0; i < 8*0x100/4; ++i) {
        mem[i] = 0xABCD0000 + i;
    }

    out->dims = 2;
    out->size[0] = 0x100;
    out->size[1] = 8;
    out->bits_per_element = 1;
    out->base = MEMORY_BASE + MEMORY_SIZE - 8*0x100 - 64;
}

int test_check(struct tensor* out, void* mem_mm, uint8_t* dla_mmio)
{
    uint32_t* status = (uint32_t*)(dla_mmio + NVDLA_GLB_S_INTR_STATUS_0);

    int32_t loop = 100000;
    volatile uint32_t int_status;
    while( 1 ) {
        int_status = *status;
        if( (int_status >> NVDLA_GLB_S_INTR_STATUS_0_BDMA_DONE_STATUS0_SHIFT) & 1 ) {
            printf("Get BDMA interrupt ...\n");
            break;
        }
        if(loop-- <= 0) {
            printf("Polling timeout\n");
            return -1;
        }
    }

    int i = 0;
    uint32_t* mem = (uint32_t*)((uint8_t*)mem_mm + out->base - MEMORY_BASE);
    for (i = 0; i < 8*0x100/4; ++i) {
        if (mem[i] != 0xABCD0000 + i) {
            printf("Result mismatch mem[%d] = 0x%x, expect 0x%x\n", i, mem[i], 0xABCD0000+i);
            return -1;
        }
    }
    return 0;
}

/*
 *  Fill DLA register configs. (test specific)
 */
const struct dla_reg* test_program(struct tensor* in, struct tensor* out)
{
    static struct dla_reg program[] = {
        {NVDLA_BDMA_CFG_SRC_ADDR_HIGH_0, 0    }, /* set later                 */
        {NVDLA_BDMA_CFG_SRC_ADDR_LOW_0 , 0    }, /* set later                 */
        {NVDLA_BDMA_CFG_DST_ADDR_HIGH_0, 0    }, /* set later                 */
        {NVDLA_BDMA_CFG_DST_ADDR_LOW_0 , 0    }, /* set later                 */
                                                                               
        {NVDLA_BDMA_CFG_DST_SURF_0     , 0x800}, /* DST SURF STRIDE           */
        {NVDLA_BDMA_CFG_DST_LINE_0     , 0x100}, /* DST LINE STRIDE           */
        {NVDLA_BDMA_CFG_SRC_SURF_0     , 0x800}, /* SRC SURF STRIDE           */
        {NVDLA_BDMA_CFG_SRC_LINE_0     , 0x100}, /* SRC LINE STRIDE           */
        {NVDLA_BDMA_CFG_SURF_REPEAT_0  , 0    }, /* SURF REPEAT NUM: 0 + 1 = 1*/
        {NVDLA_BDMA_CFG_LINE_REPEAT_0  , 7    }, /* LINE REPEAT NUM: 7 + 1 = 8*/
        {NVDLA_BDMA_CFG_LINE_0         , 7    }, /* LINE SIZE: (7+1)*32 = 256B*/
        {NVDLA_BDMA_CFG_CMD_0          , 3    }, /* DST=MC, SRC=MC            */
        {NVDLA_BDMA_CFG_OP_0           , 1    },
        {NVDLA_BDMA_CFG_LAUNCH0_0      , 1    },
        {-1, 0}
    };

    program[0].value = HIGH32( in->base  );
    program[1].value = LOW32 ( in->base  );
    program[2].value = HIGH32( out->base );
    program[3].value = LOW32 ( out->base );
    return program;
}


int main(int argc, char* argv[])
{
    void* mem_mm = mmap_device(eDEVICE_MEM);
    void* dla_mm = mmap_device(eDEVICE_DLA);

    struct tensor in, out;
    test_data(&in, &out, mem_mm);
    const struct dla_reg* test = test_program(&in, &out);

    uint8_t* const dla_mmio = (uint8_t*)dla_mm;
    uint32_t* reg = NULL;

    /* Program DLA hardware by MMIO */
    printf("Start programming...\n");
    const struct dla_reg* progs = test;
    while (progs->offset != -1) {
        reg = (uint32_t*)(dla_mmio + progs->offset);
        *reg = progs->value;
        printf("Write reg 0x%08x, value 0x%08x\n", progs->offset, progs->value);
        ++progs;
    }
    printf("Finish programming...\n\n");

    /* 
       Check regs
    printf("Start checking...\n");
    progs = test;
    while (progs->offset != -1) {
        reg = (uint32_t*)(dla_mmio + progs->offset);
        printf("Read reg 0x%08x, value 0x%08x\n", progs->offset, *reg);
        if (progs->value != *reg) {
            printf("data mismatch: expected 0x%08x\n", progs->value);
            break;
        }
        ++progs;
    }
    printf("Finish checking...\n");
    */

    /*  Check result */
    /*
    int result = 1;
    */
    int result = test_check(&out, mem_mm, dla_mmio);

    munmap_device(eDEVICE_MEM, mem_mm);
    munmap_device(eDEVICE_DLA, dla_mm);

    if (result < 0) {
        return EXIT_FAILURE;
    }
    return 0;
}
