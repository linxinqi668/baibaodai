#ifndef __OPERAND_H__
#define __OPERAND_H__

enum { OP_TYPE_REG, OP_TYPE_MEM, OP_TYPE_IMM };

#define OP_STR_SIZE 40

typedef struct {
	uint32_t type; // 操作数的种类.
	size_t size; // 操作数的大小 有些不会更新.
	union {
		uint32_t reg; // 寄存器的编号
		swaddr_t addr; // 内存的地址
		uint32_t imm; // 立即数的大小
		int32_t simm; // 有符号立即数的大小
	};
	uint32_t val; // 操作数的值
	char str[OP_STR_SIZE]; // 用于打印信息.
} Operand;

typedef struct {
	uint32_t opcode;
	bool is_operand_size_16;
	Operand src, dest, src2;
} Operands;

#endif
