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

void print_reverse(char * info) {
	int len = strlen(info);
	int i;
	for (i = len - 1; i > 0; i -= 2)
		printf("%c%c ", info[i-1], info[i]);
	printf("\n");
}

typedef struct {
	swaddr_t prev_ebp_addr; // 前面的ebp所存储的地址
	swaddr_t ret_addr; // 返回地址
	uint32_t args[4]; // 函数参数
} StackFrame;



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

// 可以重新组织一下代码, 处理args的特殊情况有点混乱。
static int cmd_info(char *args) { 

	if (args == NULL) {
		printf("请输入: info w or info r.\n");
		return 0;
	}

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
						register_value = cpu.gpr[j-4]._8[1]; // 找到一个bug
				}
				
				// 打印值
				printf("%s: %x    ", register_name[i][j], register_value);
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
		char * type = strtok(args, " ");

		if (strcmp(type, "w") != 0) {
			printf("请输入info w 或者 info -a.\n");
			return 0;
		}
		char * choice = strtok(NULL, " ");
		// printf("%s\n", choice);
		if (choice == NULL) {
			printf("请输入info w -a 或者 info w -c.\n");
			return 0;
		}
		if (strcmp(choice, "-c") == 0) { // 只打印变化的监视点
			int cnt = 0;
			while (wp) {
				if (wp->old_value != wp->now_value) {
					printf("监视点编号: %d, 值的变化为: %x -> %x\n", wp->NO, wp->old_value,
					        wp->now_value);
					printf("此时eip为: %x\n", cpu.eip);
					++cnt;
				}
				wp = wp->next;
			}
			if (cnt == 0) {
				printf("无变化的监视点.\n");
			}
		} else if (strcmp(choice, "-a") == 0) { // 打印所有监视点
			while (wp) {
				printf("监视点编号: %d, 其值为: %u -> %u\n", wp->NO, wp->old_value,
					    wp->now_value);
				wp = wp->next;
			}
		} else
			printf("info w -c, 或者 info w -a.\n");
	}
	return 0;
}

static int cmd_x(char *args){

	if (args == NULL) {
		printf("请输入: x number_of_dword addr.\n");
		return 0;
	}

	char * first_arg = strtok(args, " ");

	if (first_arg == NULL) {
		printf("请输入: x number_of_dword addr.\n");
		return 0;
	}

	char * second_arg = strtok(NULL, " ");

	if (second_arg == NULL) {
		printf("请输入: x number_of_dword addr.\n");
		return 0;
	}
	
	// printf("%s %s\n", first_arg, second_arg);

	// 转换为所需数据
	size_t num;
	swaddr_t st_addr;
	sscanf(first_arg, "%lu", &num);
	bool * is_valid = (bool *) malloc(1);
	st_addr = expr(second_arg, is_valid); // 表达式求值

	if (!is_valid) {
		printf("请输入合理的表达式!\n");
		free(is_valid);
		return 0;
	}

	free(is_valid);

	// printf("%lu %u\n", num, st_addr);

	// 打印内存数据
	uint32_t res;
	int i;
	char * buf = (char *) malloc(100);
	for (i = 0; i < num; i++) {
		res = swaddr_read(st_addr, 4);
		// 输出
		sprintf(buf, "%08x", res);
		printf("%08x: 0x ", st_addr);
		print_reverse(buf);
		// 更新起始地址
		st_addr = st_addr + 4;
	}
	free(buf);
	printf("\n");
	return 0;
}

static int cmd_p(char *args){
	
	if (args == NULL) {
		printf("请输入正确表达式.\n");
		return 0;
	}

	bool * is_valid = (bool *) malloc(1); // 判断是否是合法的表达式
	// printf("%sx\n", args);
	int res = expr(args, is_valid);
	if (*is_valid){
		free(is_valid);
		printf("0x%x\n", res);
		return 0; // 返回0，不会终端main loop;
	}
	else {
		free(is_valid);
		printf("表达式输入有误?\n");
		return 0;
	}
}

static int cmd_w(char *args) {
	if (args == NULL) {
		printf("请输入: w + expression.\n");
		return 0;
	}
	bool * is_valid = (bool *) malloc(1);
	// 申请一个监视点
	WP* new_point = new_wp();
	new_point->expr = (char *) malloc( strlen(args) );
	strcpy(new_point->expr, args); // 复制表达式
	new_point->old_value = expr(args, is_valid); // 求值
	new_point->now_value = new_point->old_value;

	Assert(*is_valid == true, "表达式无法求值!\n");
	free(is_valid);

	return 0; // 不退出main loop;
}

static int cmd_free(char *args) {
	
	WP* pool = get_pool();

	if (args == NULL) {
		printf("请输入如下指令释放:\n");
		printf("d a 删除所有监视点.\n");
		printf("d wp_code 删除编号为wp_code的监视点.\n");
		return 0;
	}

	if (strcmp(args, "a") == 0) { // 删除所有监视点
		int i;
		int num = get_num();
		for (i = 0; i < num; i++)
			free_wp(&pool[i]);
		// printf("here\n");
		return 0;
	}

	// 获取节点编号
	int wp_code = atoi(args);

	// 检查节点是否为忙碌
	if (pool[wp_code].status == false) {
		printf("不存在该监视点.\n");
		return 0;
	}
	
	// 释放节点
	free_wp(&pool[wp_code]);
	return 0;

}

static int cmd_bt(char *args) {
	// 创建当前栈帧.
	// 调用函数前, 先push参数, 然后是返回地址, 最后是ebp.
	StackFrame __this__;
	__this__.prev_ebp_addr = cpu.ebp;
	int i;

	while (__this__.prev_ebp_addr != 0) {
		// 读取当前栈帧的信息.
		// 返回地址.
		__this__.ret_addr = swaddr_read(__this__.prev_ebp_addr + 4, 4);
		// 参数.
		for (i = 0; i < 4; i++) {
			printf("%x\n", __this__.prev_ebp_addr + 8 + 4 * i);
			__this__.args[i] = swaddr_read(__this__.prev_ebp_addr + 8 + 4 * i, 4);
		}

		// 打印栈帧 暂时不打印函数名.
		printf("---------\n");
		printf("prev_ebp is: %x\n", __this__.prev_ebp_addr);
		printf("ret_addr is: %x\n", __this__.ret_addr);
		printf("4 parameters: ");
		for (i = 0; i < 4; i++)
			if (i == 0)
				printf("%x", args[i]);
			else
				printf(" %x", args[i]);
		printf("\n");
		printf("---------\n");

		// 更新ebp.
		__this__.prev_ebp_addr = swaddr_read(__this__.prev_ebp_addr, 4);
	}

	return 0;
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
	{ "si", "Exec n instructions.", cmd_si},
	// 查看信息
	{ "info", "Check the or watch_point infomation.", cmd_info},
	// 扫描内存
	{ "x", "Print n * 4 bytes after the input address.", cmd_x},
	// 表达式求值
	{ "p", "Get the expression's value.", cmd_p},
	// 设置监视点
	{ "w", "Set a watch_point.", cmd_w},
	// 释放监视点
	{ "d", "Input a integer or 'a' to free a watch_point.", cmd_free},
	// 打印栈帧链
	{ "bt", "Print the stack chain.", cmd_bt}
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
