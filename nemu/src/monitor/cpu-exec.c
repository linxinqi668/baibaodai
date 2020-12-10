#include "monitor/monitor.h"
#include "cpu/helper.h"
#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#include <setjmp.h>
#include <stdlib.h>


/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10

int nemu_state = STOP;

int exec(swaddr_t);

char assembly[80];
char asm_buf[128];

/* Used with exception handling. */
jmp_buf jbuf;

char* running_message [] = {
	// PART 1
	"                 The Road Not Taken 《少有人走的路》\n\n",
	"                 Two roads diverged in a yellow wood,\n\
             黄色的树林里分出两条路，黄色的树林里分出两条路，\n\n",
	"                  And sorry I could not travel both\n\
                        可惜我不能同时去涉足，\n\n",
	"                   And be one traveler,long I stood\n\
                        我在那路口久久伫立，\n\n",
	"                 And looked down one as far as I could\n\
                        我向着一条路极目望去，\n\n",
	"                  To where it bent in the undergrowth;\n\
                          直到它消失在丛林深处。\n\n",

	// PART 2
	"                  Then took the other, as just as fair,\n\
                  And having perhaps the better claim,\n\
                  Because it was grassy and wanted wear;\n\
                 它荒草萋萋，十分幽寂，显得更诱人、更美丽，\n\n",
	"                  Though as for that the passing there\n,\
                          虽然在这两条小路上，\n\n",
	"                  Had worn them really about the same,\n\
                          都很少留下旅人的足迹，\n\n",

	// PART 3
	"                   And both that morning equally lay\n\
                  In leaves no step had trodden black.\n\
               虽然那天清晨落叶满地，两条路都未经脚印污染。\n\n",
	"                  Oh, I kept the first for another day!\n\
                     呵，留下一条路等改日再见！\n\n",
	"             Yet knowing how way leads on to way,\n\
             但我知道路径延绵无尽头，\n\n",
	"             I doubted if I should ever come back.\n\
             恐怕我难以再回返。\n\n",

	 // PART 4
	"             I shall be telling this with a sigh\n\
             Somewhere ages and ages hence:\n\
             也许多少年后在某个地方，我将轻声叹息把往事回顾\n\n",
	"             Two roads diverged in a wood, and I —\n\
             一片树林里分出两条路，\n\n",
	"             I took the one less traveled by,\n\
             而我选了人迹更少的一条，\n\n",
	"             And that has made all the difference.\n\
             从此决定了我一生的道路。\n\n"

};

void print_bin_instr(swaddr_t eip, int len) {
	int i;
	int l = sprintf(asm_buf, "%8x:   ", eip);
	for(i = 0; i < len; i ++) {
		l += sprintf(asm_buf + l, "%02x ", instr_fetch(eip + i, 1));
	}
	sprintf(asm_buf + l, "%*.s", 50 - (12 + 3 * len), "");
}

/* This function will be called when an `int3' instruction is being executed. */
void do_int3() {
	printf("\nHit breakpoint at eip = 0x%08x\n", cpu.eip);
	nemu_state = STOP;
}

/* Simulate how the CPU works. */
void cpu_exec(volatile uint32_t n) {
	// output message...
	uint32_t message_num = 17;
	uint32_t message_ind = 0;

	if(nemu_state == END) {
		printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
		return;
	}
	nemu_state = RUNNING;

#ifdef DEBUG
	volatile uint32_t n_temp = n;
#endif

	setjmp(jbuf);

	for(; n > 0; n --) { // cpu执行指令的循环
#ifdef DEBUG
		swaddr_t eip_temp = cpu.eip;
		if((n & 0xffff) == 0) {
			/* Output some dots while executing the program. */
			// fputc('.', stderr);
			if (message_ind < message_num) {
				fputs(running_message[message_ind], stderr);
				message_ind++;
			}
			else
				fputs("....if you see this, it means nmeu is too slow.\n",
				      stderr);
		}
#endif

		/* Execute one instruction, including instruction fetch,
		 * instruction decode, and the actual execution. */
		int instr_len = exec(cpu.eip); // 执行一条指令

		cpu.eip += instr_len;



#ifdef DEBUG
		print_bin_instr(eip_temp, instr_len);
		strcat(asm_buf, assembly);
		Log_write("%s\n", asm_buf);
		if(n_temp < MAX_INSTR_TO_PRINT) {
			printf("%s\n", asm_buf);
		}
#endif

		/* TODO: check watchpoints here. */
		// 计算所有表达式的值
		WP* watch_point = get_head(); // 头指针, 如果是空的说明没有监视点, 直接会跳过下一段。
		bool exist_change = false;
		while (watch_point) {
			watch_point->old_value = watch_point->now_value;
			bool * is_valid = (bool *) malloc(1);
			watch_point->now_value = expr(watch_point->expr, is_valid);
			Assert(*is_valid == true, "表达式无法求值.");

			free(is_valid);

			if (watch_point->old_value != watch_point->now_value)
				exist_change = true;
				
			watch_point = watch_point->next;
		}

		if (exist_change) {
			nemu_state = STOP;
			printf("监视点发生变化, 请输入指令查看.\n");
		}

		// cpu更新eip后检测是否到达断点, 所以断点处的代码未执行。
		// 使用 si 指令 或者 c 指令可继续执行。


#ifdef HAS_DEVICE
		extern void device_update();
		device_update();
#endif

		if(nemu_state != RUNNING) { return; }
	}

	if(nemu_state == RUNNING) { nemu_state = STOP; }
}
