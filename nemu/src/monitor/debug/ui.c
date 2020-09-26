#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
// #include "reg.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>


void cpu_exec(uint32_t);

// 打印无符号整数的二进制表示



/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}

static int cmd_si(char *args) {
	char const * step_num = (char const *) args;
	int n;
	if (args == NULL)
		n = 1;
	else
		n = atoi(step_num);
	cpu_exec(n);
	return -1;
}

static int cmd_info(char *args) {
	char * register_name[3][8] = {
		{"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"},
		{"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"},
		{"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"}
	};

	if (strcmp(args, "r") == 0){

		for (int i = 0; i < 3; i++){
			for (int j = 0; j < 8; j++){
				// 取出该寄存器的值
				uint32_t register_value;
				if (i == 0)
					register_value = cpu.gpr[j]._32;
				else if (i == 1)
					register_value = cpu.gpr[j]._16;
				else {
					if (j <= 3)
						register_value = cpu.gpr[j]._8[0];
					else
						register_value = cpu.gpr[j]._8[1];
				}
				
				// 打印值
				printf("register %s: %u\n", register_name[i][j], register_value);
			}
		}
	}
	else
		printf("this is w\n");
	return -1;
}

static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },

	/* TODO: Add more commands */
	// 多步执行指令, 缺省为1步。
	{ "si", "N Step Further", cmd_si},
	// 查看信息
	{ "info", "Check The Register Or Watchpoint Infomation", cmd_info},

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
