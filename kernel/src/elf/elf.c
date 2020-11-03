#include "common.h"
#include "memory.h"
#include <string.h>
#include <elf.h>
#include <stdio.h>

#define ELF_OFFSET_IN_DISK 0

#ifdef HAS_DEVICE
void ide_read(uint8_t *, uint32_t, uint32_t);
#else
void ramdisk_read(uint8_t *, uint32_t, uint32_t);
#endif

#define STACK_SIZE (1 << 20)

void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader() {
	printf("here!!!!!!!!!!\n");
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph = NULL; // 程序头数组

	uint8_t buf[4096];
	printf("%s\n", (char *)buf);

#ifdef HAS_DEVICE
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
#else
	ramdisk_read(buf, ELF_OFFSET_IN_DISK, 4096); // 已经读取了4096个字节了.
#endif

	elf = (void*)buf;

	/* TODO: fix the magic number with the correct one */
	
	// 前4个字节就是魔数.
	const uint32_t * p = (const uint32_t *)buf; // 指向前4个字节.
	const uint32_t elf_magic = *p; // 然后读取.

	uint32_t *p_magic = (void *)buf; // 测试.
	nemu_assert(*p_magic == elf_magic);

	// (1) 取出程序头表的偏移(地址).
	uint32_t ph_offset = elf->e_phoff;
	// (2) 更新ph.
	ph = (Elf32_Phdr *)(buf + ph_offset);
	// (3) 取出程序头的个数.
	uint32_t ph_num = elf->e_phnum;
	


	/* Load each program segment */
	// panic("please implement me");
	uint32_t cnt = 0;
	for(; cnt < ph_num; cnt++) {
		/* Scan the program header table, load each segment into memory */
		if(ph->p_type == PT_LOAD) {

			printf("here!!!!!!!!!!\n");
			/* TODO: read the content of the segment from the ELF file 
			 * to the memory region [VirtAddr, VirtAddr + FileSiz)
			 */

			// 1. 只加载type为LOAD的segment.
			// 2. 每个segment都有自己的物理地址, 以及file size, memsize.
			// 3. 每个文件的开头都是0x0. 读取前4个字节就是魔数.
			// 仿照另一个elf.c文件就可以写出来了.
			uint8_t * st_addr = (uint8_t *)ph->p_vaddr;
			printf("%x\n", (unsigned int)st_addr);
			uint32_t mem_size = ph->p_memsz;
			uint32_t file_size = ph->p_filesz;

			// 写入内存.
			memcpy(st_addr, (uint8_t *)ph, file_size);
			
			
			/* TODO: zero the memory region 
			 * [VirtAddr + FileSiz, VirtAddr + MemSiz)
			 */
			// 清零内存空间.
			memset(st_addr + file_size, 0, mem_size - file_size);

			// 更新ph.
			ph++;


#ifdef IA32_PAGE
			/* Record the program break for future use. */
			extern uint32_t cur_brk, max_brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if(cur_brk < new_brk) { max_brk = cur_brk = new_brk; }
#endif
		}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);

#ifdef HAS_DEVICE
	create_video_mapping();
#endif

	write_cr3(get_ucr3());
#endif

	return entry;
}
