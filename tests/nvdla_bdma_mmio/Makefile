GCC=aarch64-linux-gnu-gcc
NVDLA_CMOD_INCLUDE = ../../models/nvdla_hw_cmod/cmod/include

all: bdma

bdma:
	$(GCC) -o aarch64_nvdla_bdma_mmio nvdla_bdma_mmio.c -I$(NVDLA_CMOD_INCLUDE) -Wall

clean:
	rm -f aarch64_nvdla_bdma_mmio

