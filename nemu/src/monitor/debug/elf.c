#include "common.h"
#include <stdlib.h>
#include <elf.h>

char *exec_file = NULL;

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL; // 符号表.
static int nr_symtab_entry; // 符号表的表项数.


char * get_strtab() {
	return strtab;
}

Elf32_Sym * get_symtab() {
	return symtab;
}

int get_symtab_len() {
	return nr_symtab_entry;
}

char * get_fun_name(swaddr_t addr) {
	// 查找函数.
	int i;
	char * fun_name = (char *)malloc(30);
	// printf("reached this line1\n");
	for (i = 0; i < nr_symtab_entry; i++)
		if ((symtab[i].st_info & 0x0f) == STT_FUNC) // type is function.
			if (symtab[i].st_value <= addr &&
				addr <= symtab[i].st_value + symtab[i].st_size) {
					// 计算函数名的长度. 符号表中是\0结尾.
					uint32_t fun_len = strlen(strtab + symtab[i].st_name);
					// printf("len is %d\n", fun_len);
					// 取出函数名.
					strncpy(fun_name, strtab + symtab[i].st_name, fun_len);
					fun_name[fun_len] = '\0';
					// ok.
				}
	return fun_name;
}




void load_elf_tables(int argc, char *argv[]) {
	int ret;
	Assert(argc == 2, "run NEMU with format 'nemu [program]'");
	exec_file = argv[1];

	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	uint8_t buf[sizeof(Elf32_Ehdr)];
	ret = fread(buf, sizeof(Elf32_Ehdr), 1, fp); // 读取文件头. 直接读取一个结构体。学到了学到了.
	assert(ret == 1);

	/* The first several bytes contain the ELF header. */
	Elf32_Ehdr *elf = (void *)buf;
	char magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

	/* Check ELF header */
	assert(memcmp(elf->e_ident, magic, 4) == 0);		// magic number
	assert(elf->e_ident[EI_CLASS] == ELFCLASS32);		// 32-bit architecture
	assert(elf->e_ident[EI_DATA] == ELFDATA2LSB);		// littel-endian
	assert(elf->e_ident[EI_VERSION] == EV_CURRENT);		// current version
	assert(elf->e_ident[EI_OSABI] == ELFOSABI_SYSV || 	// UNIX System V ABI
			elf->e_ident[EI_OSABI] == ELFOSABI_LINUX); 	// UNIX - GNU
	assert(elf->e_ident[EI_ABIVERSION] == 0);			// should be 0
	assert(elf->e_type == ET_EXEC);						// executable file
	assert(elf->e_machine == EM_386);					// Intel 80386 architecture
	assert(elf->e_version == EV_CURRENT);				// current version


	/* Load symbol table and string table for future use */

	/* Load section header table */
	// 计算出节头的大小.
	uint32_t sh_size = elf->e_shentsize * elf->e_shnum;
	// 分配空间.
	Elf32_Shdr *sh = malloc(sh_size);
	// 查找节头表的offset. 这很重要. 我猜更改了fp指针的位置.
	fseek(fp, elf->e_shoff, SEEK_SET);
	// 然后从新的fp指针这里读取.
	ret = fread(sh, sh_size, 1, fp);
	assert(ret == 1);

	/* Load section header string table */
	char *shstrtab = malloc(sh[elf->e_shstrndx].sh_size);
	fseek(fp, sh[elf->e_shstrndx].sh_offset, SEEK_SET);
	ret = fread(shstrtab, sh[elf->e_shstrndx].sh_size, 1, fp);
	assert(ret == 1);

	int i;
	for(i = 0; i < elf->e_shnum; i ++) {
		if(sh[i].sh_type == SHT_SYMTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {
			/* Load symbol table from exec_file */
			symtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(symtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
			nr_symtab_entry = sh[i].sh_size / sizeof(symtab[0]);
		}
		else if(sh[i].sh_type == SHT_STRTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
			/* Load string table from exec_file */
			strtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(strtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
		}
	}

	free(sh);
	free(shstrtab);

	assert(strtab != NULL && symtab != NULL);
	// printf("%s!!!!!!!!!", strtab);

	fclose(fp);
}

