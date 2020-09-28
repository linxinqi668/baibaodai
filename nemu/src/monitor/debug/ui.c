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
// my_self
char * print_binary_vector(unsigned int val){
    char * res = (char *) malloc(33); // 分配32个字节的空间
	int i;
    for (i = 31; i >= 0; i--){
        // 取出当前数字
        int x = val % 2;
        // 放入res字符串中
        *(res + i) = '0' + x;
        val = val / 2;
    }
    // 加上结束符
    *(res + 32) = '\0';

    return res;
}



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
	return 0;
}

static int cmd_info(char *args) {
	char * register_name[3][8] = {
		{"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"},
		{"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"},
		{"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"}
	};

	if (strcmp(args, "r") == 0){

		int i, j;
		for (i = 0; i < 3; i++){
			for (j = 0; j < 8; j++){
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
				printf("%s: %u    ", register_name[i][j], register_value);
				char * vec = print_binary_vector(register_value);
				printf("bin_vec: %s\n", vec);
				// 释放内存
				free(vec);
			}
		}
	}
	else { // 显示监视点的信息
		WP* wp = get_head(); // 获取头指针
		if (wp == NULL) {
			printf("无监视点.\n");
			return 0;
		}
		strtok(args, " ");
		char * choice = strtok(NULL, " ");
		if (strcmp(choice, "-c") == 0) { // 只打印变化的监视点
			while (wp) {
				if (wp->old_value != wp->now_value) {
					printf("监视点编号: %d, 值的变化为: %u -> %u\n", wp->NO, wp->old_value,
					        wp->now_value);
				}
			}
		} else if (strcmp(choice, "-a") == 0) { // 打印所有监视点
			while (wp) {
				printf("监视点编号: %d, 其值为: %u -> %u\n", wp->NO, wp->old_value,
					    wp->now_value);
			}
		} else 
			printf("info w -c, 或者 info w -a.\n");
	}
	return 0;
}

static int cmd_x(char *args){
	printf("兄弟! 你需要以字节为单位反过来看每一个小串, 然后再连起来, 可得到\n\
低地址到高地址的内存表示, 本机是大端序.\n\n");

	char * first_arg = strtok(args, " ");
	char * second_arg = strtok(NULL, " ");
	
	// printf("%s %s\n", first_arg, second_arg);

	// 转换为所需数据
	size_t num;
	swaddr_t st_addr;
	sscanf(first_arg, "%lu", &num);
	sscanf(second_arg, "%x", &st_addr);

	// printf("%lu %u\n", num, st_addr);

	// 打印内存数据
	uint32_t res;
	int i;
	for (i = 0; i < num; i++) {
		res = swaddr_read(st_addr, 4);
		// 输出
		printf("%08x: 0x%08x\n", st_addr, res);
		// if ( (i+1) % 5 == 0)
		// 	printf("\n");
		// 更新起始地址
		st_addr = st_addr + 4;
	}
	printf("\n");
	return 0;
}

static int cmd_p(char *args){
	// long long res = 0;
	bool * is_valid = (bool *) malloc(1); // 判断是否是合法的表达式
	expr(args, is_valid); // 结果通过expr函数打印
	if (*is_valid){
		free(is_valid);
		return 0; // 返回0，不会终端main loop;
	}
	else {
		free(is_valid);
		printf("表达式输入有误?\n");
		return 0;
	}
}

static int cmd_w(char *args) {
	bool * is_valid = (bool *) malloc(1);
	// 申请一个监视点
	WP* new_point = new_wp();
	new_point->expr = args;
	new_point->old_value = expr(args, is_valid); // 求值
	new_point->now_value = new_point->old_value;

	Assert(*is_valid == true, "表达式无法求值!\n");
	free(is_valid);

	return 0; // 不退出main loop;
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
	// 扫描内存
	{ "x", "Print N * 4 Bytes After The Input Address", cmd_x},
	// 表达式求值
	{ "p", "Get The Expression Value", cmd_p},
	// 设置监视点
	{ "w", "Set A WatchPoint", cmd_w}
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
