#include "common.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

#include "cache/cache.h"


/* Memory accessing interfaces */

uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	uint32_t answer = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	// return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	uint32_t cache_ans = M_CACHE.m_cache_read(&M_CACHE, addr, len);
	printf("ans: %x cache: %x\n\n\n", answer, cache_ans);
	// assert(answer == cache_ans);
	return answer;
}

void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	// dram_write(addr, len, data);
	M_CACHE.m_cache_write(&M_CACHE, addr, data, len);
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
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

