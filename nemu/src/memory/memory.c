#include "common.h"
// #include "nemu/include/cpu/reg.h"
#include "nemu.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#include "cache/L1-cache.h"


lnaddr_t seg_translate(swaddr_t addr,size_t len,uint8_t sreg_id){
	if (cpu.cr0.protect_enable == 0) return addr;
	else {
		return cpu.sreg[sreg_id].base + addr;
	}
}


/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	// uint32_t answer = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	// return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	uint32_t cache_ans = L1_M_CACHE.m_cache_read(&L1_M_CACHE, addr, len);
	// printf("ans: %x cache: %x\n\n\n", answer, cache_ans);
	// assert(answer == cache_ans);
	return cache_ans;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	// dram_write(addr, len, data);
	L1_M_CACHE.m_cache_write(&L1_M_CACHE, addr, data, len);
}

uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr,len,current_sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr,len,current_sreg);
	lnaddr_write(lnaddr, len, data);
}

