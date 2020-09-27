#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
#include <stdlib.h>

enum {
	NOTYPE = 256, EQ, Integer = 'i', Left = '(', Right = ')',
	Multiply = '*', Div = '/', Plus = '+', Sub = '-'

	/* TODO: Add more token types */
};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	// token_type is an integer, so use a char to represent it.

	// 1st level
	{" +",	NOTYPE},				// spaces
	{"[0-9]+", Integer},             // get an integer 这里可能需要更改

	// 2nd level
	{"\\(", Left},                  // left parenthesis
	{"\\)", Right},                 // right parenthesis

	// 3rd level
	{"\\*", Multiply},              // multiply
	{"/", Div}, 					// div

	// 4th level
	{"\\+", Plus},					// plus
	{"-", Sub},                     // subtract
	{"==", EQ},						// equal
	

};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

// 单元结构体
typedef struct token {
	int type;
	char * str; // 为了避免缓冲区溢出, 直接利用堆的空间即可
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0; // 当前扫描的位置
	int i;
	regmatch_t pmatch;
	
	nr_token = 0; // token计数器

	// printf("%s\n", e); 没问题

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position; // 子串的起始位置
				int substr_len = pmatch.rm_eo; // 子串的长度

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				printf("%s\n", e + position);
				printf("%d\n", substr_len);

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				
				// 丢弃空格
				if ( rules[i].token_type == NOTYPE)
					continue;
				
				// 记录token的类型
				tokens[ nr_token ].type = rules[i].token_type;

				Assert(substr_len <= 31, "oh ho buf overflow!");

				// 开辟一块新的空间来保存该子串, 留一个字符作为结束符
				char * new_space = (char *)malloc(substr_len + 1);
				strncpy(new_space, substr_start, substr_len);
				new_space[substr_len] = '\0'; // strcpy不会添加上\0结束符

				// int l = strlen(new_space);
				// printf("%d\n !!!!", l);
				// printf("%s\n", new_space);

				// 更新指针
				tokens[ nr_token ].str = new_space;
				// 确保字符串相等
				Assert(strlen(new_space) == substr_len, "copy failed!");

				// 更新计数器
				nr_token++;

				Assert(nr_token <= 32, "the tokens array over flow!");
				

				switch(rules[i].token_type) {
					// default: panic("please implement me");
				}

				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

// stack

uint32_t expr(char *e, bool *success) {
	// 记得释放tokens中的字符串空间

	// 如果找不到正常的表达式就直接退出
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	*success = true;

	/* TODO: Insert codes to evaluate the expression. */

	// 前面已经进行过了make_token

	// 拼接表达式
	char * expression = (char *)malloc(100);
	memset(expression, 0, 100);

	for (int i = 0; i < nr_token; i++)
		strcat(expression, tokens[i].str);

	printf("%s\n", expression);

	

	// 测试部分, 打印中取出的token
	// for (int i = 0; i < nr_token; i++){
	// 	printf("%s ", tokens[i].str);
	// }


	// 释放tokens中的字符串空间
	for (int i = 0; i < nr_token; i++)
		free(tokens[i].str);

	// panic("please implement me");
	return 0;
}

