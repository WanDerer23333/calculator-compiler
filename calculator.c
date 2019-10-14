#include<string.h>
#include<stdio.h>
#include<stdlib.h>

#define MAX_STACK_SIZE 500	//表达式栈大小
#define MAX_VAR_NUM 50		//变量个数
#define MAX_VAR_NAME_LENGTH 30	//变量名长度
#define N 200	//单行长度

enum Tag {
	LINE,	//句结束	0
	END, 	//全文结束	1
	SIGN, 	//计算符	2
	EQUAL,	//等号		3
	INT, 	//整数		4
	FLOAT, 	//浮点数	5
	INIT_D, //声明整型	6	
	INIT_F, //声明浮点	7
	FUNC,	//函数		8
	VAR 	//变量 		9
};
enum Sign {
	PLUS = '+', 
	MINUS = '-', 
	MUL = '*', 
	DIV = '/', 
	LP = '(', 
	RP = ')'
};
typedef struct _Token { //tag表示属性， value为具体值
	int tag;
	char name[MAX_VAR_NAME_LENGTH];
	union Value {
		int sign;
		int i_val;
		float f_val;
	} value;
} Token;
typedef struct _Var { //name表示变量名，tag表示类型， value为具体值, filled为1说明已赋值
	char name[MAX_VAR_NAME_LENGTH];
	int filled;
	int tag;
	union Val {
		int i_val;
		float f_val;
	} value;
} Var;
typedef struct _Varlist { //var为变量列表， num为当前声明变量个数
	Var var[MAX_VAR_NUM];
	int num;
} Varlist;
typedef struct _Stack	//栈，用于表达式计算
{
	Token *top;
	Token *base;
	int size;
}Stack;


Varlist varlist;//全局
int line_num = 0;
Token Tokens[MAX_STACK_SIZE];
Token expr[MAX_STACK_SIZE];



int setTokens (char* str);	//取一行输入中包含的token存入Tokens
void var_init (Token var_token, int init_tag);	//变量声明
void var_assgin (Token var_token, Token num_token);	//变量赋值
Token var_value (Token var_token);	//变量取值，返回一个Token类型
int setExpr (int location);	//取算术表达式到expr，返回表达式长度
int get_op_level(int op);	//比较运算符优先级
void init_stack(Stack *stack);	//栈初始化
void free_stack(Stack *stack);	//栈清空
void push(Stack *stack, Token t);	//t元素入栈
Token* pop(Stack *stack);	//出栈
Token* top(Stack *stack);	//查看栈顶值
void inverse(Stack *stack);	//反转栈
void cal_token(Token* t1, Token* t2, int sign, Token* result);	//根据sign计算结果存入result指向的位置
void error (char* msg);		//报错
Token cal (int len);		//len为expr表达式长度，计算该表达式结果存入token并返回


int main(int argc, char *argv[]) {
	varlist.num = 0;
    FILE *fp;
    char str[N + 1];
    if ((fp = fopen(/*"d:\\demo2.txt"*/argv[1], "rt")) == NULL ) {
        puts("Fail to open file!");
        exit(0);
    }
    while(fgets(str, N, fp) != NULL ) {
    	line_num++;	//行号加一
    	int j = setTokens(str);
    	if (j == 0) {
    		continue;
		} 
		if (Tokens[j - 1].tag != LINE && Tokens[j - 1].tag != END) {
			error ("syntax error"); 
		}
    	int i = 0;
    	int end = 0;
    	/*for (i = 0; i < j; i++) {
    		printf("%d", Tokens[i].tag);
    	}
    	printf("\n");*/

		if (Tokens[i].tag == END) {
			end = 1;
			break;
		} else if (Tokens[i].tag == LINE) {
			continue;
		} else if (Tokens[i].tag == INIT_D || Tokens[i].tag == INIT_F) {
			if (Tokens[++i].tag == VAR) {
				if (Tokens[i + 1].tag == LINE || Tokens[i + 1].tag == END) {
					if (Tokens[i - 1].tag == INIT_D) {
						var_init (Tokens[i], INT);
					} else {
						var_init (Tokens[i], FLOAT);
					}
					
				} else {
					error("syntax error");
				}
			} else {
				error ("syntax error"); 
			}
		} else if (Tokens[i].tag == VAR) {
			if (Tokens[++i].tag == EQUAL) {
				int expr_len = setExpr(i + 1);
				Token result = cal(expr_len);
				var_assgin (Tokens[i - 1], result);
			} else {
				error("syntax error");
			}
		} else if (Tokens[i].tag == FUNC) {
			if(Tokens[i + 1].tag == SIGN && Tokens[i + 1].value.sign == '(' 
				&& Tokens[i + 3].tag == SIGN && Tokens[i + 3].value.sign == ')') {
				if (Tokens[i + 2].tag == VAR) {
					Token result = var_value (Tokens[i + 2]);
					if (result.tag == INT) {
						printf("%d\n", result.value.i_val);
					} else {
						printf("%.2f\n", result.value.f_val);
					}
				} else {
					error("syntax error");
				} 
			} else {
				error("syntax error");
			}
		}
    }
    fclose(fp);
    return 0;
}


int setTokens (char* str) {	//取一行输入中包含的token存入Tokens
	int i = 0;	//遍历str
	int j = 0;	//遍历Tokens
	char curr_c;	//当前字符
	while (str[i] != '\n') {
		curr_c = str[i];
		if (curr_c == '+' || curr_c == '-' || curr_c == '*' 
			|| curr_c == '/' || curr_c == '(' || curr_c == ')') {	//检查SIGN运算符
			Tokens[j].tag = SIGN;
			Tokens[j].value.sign = curr_c;
			j++;
			i++;
		} else if (curr_c >= '0' && curr_c <= '9') {	//检查数字，INT或FLOAT
			int is_float = 0;
			int int_val = 0;
			float float_val;
			do {
				int_val = int_val * 10 + (curr_c - '0');
				curr_c = str[++i];
			} while (curr_c >= '0' && curr_c <= '9');

			if (curr_c == '.') {
				float_val = int_val;
				curr_c = str[++i];
				float times = 10.0f;
				int overflow = 0;
				while (curr_c >= '0' && curr_c <= '9') {
					if (overflow) {
						curr_c = str[++i];
						continue;
					}
					float_val += (curr_c - '0') / times;
					curr_c = str[++i];
					times *= 10;
					// 防止溢出
					if (times > 0xffffffffu / 10) {
						overflow = 1;
					}
				}
				Tokens[j].tag = FLOAT;
				Tokens[j].value.f_val = float_val;
			} else {
				Tokens[j].tag = INT;
				Tokens[j].value.i_val = int_val;
			}
			j++;
		} else if (curr_c == ' ') {	//跳过空格
			i++;
		} else if (curr_c == ';') {	//分号，表示语句结束
			Tokens[j].tag = LINE;
			j++;
			i++;
		} else if (curr_c == '.') {	//句号，表示全文结束
			Tokens[j].tag = END;
			j++;
			i++;
			break;
		} else if (curr_c == '=') {	//检查赋值
			Tokens[j].tag = EQUAL;
			j++;
			i++;
		} else if (curr_c == '_' || (curr_c >= 'a' && curr_c <= 'z')	//检查变量及关键字
			|| (curr_c >= 'A' && curr_c <= 'Z')) {
			int k = i;
			char name_unknown[MAX_VAR_NAME_LENGTH];
			while(curr_c == '_' || (curr_c >= 'a' && curr_c <= 'z')
			|| (curr_c >= 'A' && curr_c <= 'Z') || (curr_c >= '0' && curr_c <= '9')) {
				name_unknown[i - k] = curr_c;
				curr_c = str[++i];
			}
			name_unknown[i - k] = '\0';
			if (!strcmp(name_unknown, "float") && curr_c == ' ') {
				Tokens[j].tag = INIT_F;
				j++;
			} else if (!strcmp(name_unknown, "int") && curr_c == ' ') {
				Tokens[j].tag = INIT_D;
				j++;
			} else if (!strcmp(name_unknown, "write")) {
				Tokens[j].tag = FUNC;
				j++;
			} else {
				Tokens[j].tag = VAR;
				strcpy(Tokens[j].name, name_unknown);
				j++;
			}
		} else {
			error("syntax error");
		}
	}
	return j;
}

void var_init (Token var_token, int init_tag) {	//变量声明
	if (var_token.tag == VAR) {
		int i;
		for (i = 0; i < varlist.num; i++) {
			if (!strcmp(var_token.name, varlist.var[i].name)) {
				error("repetitive identifier");
			}
		}
		strcpy(varlist.var[varlist.num].name, var_token.name);
		varlist.var[varlist.num].filled = 0;
		varlist.var[varlist.num].tag = init_tag;
		varlist.num++;
	}
}
void var_assgin (Token var_token, Token num_token) {	//变量赋值
	if (var_token.tag == VAR && (num_token.tag == INT || num_token.tag == FLOAT)) {
		int i;
		for (i = 0; i < varlist.num; i++) {
			if (!strcmp(var_token.name, varlist.var[i].name)) {
				if (varlist.var[i].tag == INT) {
					varlist.var[i].filled = 1;
					if (num_token.tag == INT) {
						varlist.var[i].value.i_val = num_token.value.i_val;
					} else if (num_token.tag == FLOAT) {
						varlist.var[i].value.i_val = (int)num_token.value.f_val;
					}
				} else {
					if (num_token.tag == INT) {
						error("type cast : turn int into float");
					} else if (num_token.tag == FLOAT) {
						varlist.var[i].value.f_val = num_token.value.f_val;
						varlist.var[i].filled = 1;
					}
				}
				return;
			}
		}
		error("undefined identifier");
	}
}
Token var_value (Token var_token) {	//变量取值，返回一个Token类型
	int i;
	Token result;
	for (i = 0; i < varlist.num; i++) {
		if (!strcmp(var_token.name, varlist.var[i].name)) {
			if (varlist.var[i].filled != 1) {
				error("uninitialized identifier");
			} else {
				result.tag = varlist.var[i].tag;
				if (result.tag == INT) {
					result.value.i_val = varlist.var[i].value.i_val;
				} else {
					result.value.f_val = varlist.var[i].value.f_val;
				}
			}
			return result;
		}
	}
	error("undefined identifier");
}

int setExpr (int location) {	//取算术表达式
	int j = 0;
	while (Tokens[location].tag != LINE && Tokens[location].tag != END) {
		if (Tokens[location].tag == SIGN) {
			expr[j].tag = SIGN;
			expr[j].value.sign = Tokens[location].value.sign;
			j++;
			location++;
		} else if (Tokens[location].tag == INT) {
			expr[j].tag = INT;
			expr[j].value.i_val = Tokens[location].value.i_val;
			j++;
			location++;
		} else if (Tokens[location].tag == FLOAT) {
			expr[j].tag = FLOAT;
			expr[j].value.f_val = Tokens[location].value.f_val;
			j++;
			location++;
		} else if (Tokens[location].tag == VAR) {
			Token result = var_value(Tokens[location]);
			expr[j].tag = result.tag;
			if (result.tag == INT) {
				expr[j].value.i_val = result.value.i_val;
			} else {
				expr[j].value.f_val = result.value.f_val;
			}
			j++;
			location++;
		} else {
			error("expression error");
		}
	}
	return j;	//算术表达式token数
}

int get_op_level(int op) {	//比较运算符优先级
	switch (op)
	{
	case '(':
		return 1;
	case '+':
	case '-':
		return 2;
	case '*':
	case '/':
		return 3;
	case ')':
		return 4;
	default:
		error("bad op");
		return 0;
	}
}
void init_stack (Stack *stack) {	//栈初始化
	stack->base = malloc(MAX_STACK_SIZE * sizeof(Stack));
	stack->size = 0;
	stack->top = NULL;
}
void free_stack (Stack *stack) {	//栈清空
	free(stack->base);
}
void push (Stack *stack, Token t) { //Token元素入栈
	int size = stack->size;
	if (size >= MAX_STACK_SIZE) {
		error("stack overflow");
	}
	stack->base[size] = t;
	stack->size++;
	stack->top = stack->base + stack->size - 1;
}
Token* pop (Stack *stack) {	//出栈
	if (stack->size == 0) {
		error("can't pop empty stack");
	}
	Token *top_token = top(stack);
	stack->top = stack->top - 1;
	stack->size--;
	return top_token;
}
Token* top (Stack *stack) {	//查看栈顶元素
	return stack->top;
}
void inverse(Stack *stack) {	//反转栈
	Stack s2;
	init_stack(&s2);
	size_t i = stack->size;
	for (; i > 0; i--) {
		Token* t = pop(stack);
		push(&s2, *t);
	}
	free_stack(stack);
	stack->top = s2.top;
	stack->base = s2.base;
	stack->size = s2.size;
}
void cal_token(Token* t1, Token* t2, int sign, Token* result) {
	if (t1->tag == FLOAT || t2->tag == FLOAT) {
		result->tag = FLOAT;
		float v1, v2;
		v1 = t1->tag == FLOAT ? t1->value.f_val : t1->value.i_val;
		v2 = t2->tag == FLOAT ? t2->value.f_val : t2->value.i_val;
		switch (sign) {
		case '+':
			result->value.f_val = v1 + v2;
			return;
		case '-':
			result->value.f_val = v1 - v2;
			return;
		case '*':
			result->value.f_val = v1 * v2;
			return;
		case '/':
			if (v2 == 0.0f) {
				error("divide by zero");
			}
			result->value.f_val = v1 / v2;
			return;
		default:
			return;
		}
	}
	else {
		result->tag = INT;
		int v1, v2;
		v1 = t1->value.i_val;
		v2 = t2->value.i_val;
		switch (sign) {
		case '+':
			result->value.i_val = v1 + v2;
			return;
		case '-':
			result->value.i_val = v1 - v2;
			return;
		case '*':
			result->value.i_val = v1 * v2;
			return;
		case '/':
			if (v2 == 0) {
				error("divide by zero");
			}
			result->value.i_val = v1 / v2;
			return;
		default:
			return;
		}
	}
}

Token cal (int len) {	//计算算术表达式，返回token
	int location = 0; // 下标
	Token current;
	Stack post;
	Stack ops;
	init_stack(&post);
	init_stack(&ops);

	while (location < len) {
		current.tag = expr[location].tag;
		current.value = expr[location].value;
		switch (current.tag) {
			case INT:
			case FLOAT: {
				push(&post, current);
				break;
			}
			case SIGN: {
				int sign = current.value.sign;
				switch (sign) {
					case LP:
						push(&ops, current);
						break;
					case RP:{}
						do {
							Token* op;
							if (ops.size == 0) {
								error("parentheses mismatch");
							}
							op = pop(&ops);
							push(&post, *op);
						} while (top(&ops)->value.sign != LP);
						pop(&ops);
						break;
					case PLUS:
					case MINUS:
					case MUL:
					case DIV: {
						if (ops.size == 0) {
							push(&ops, current);
							break;
						}
						int top_op = top(&ops)->value.sign;
						if (get_op_level(sign) > get_op_level(top_op)) {
							push(&ops, current);
							break;
						} else {
							while (ops.size != 0) {
								int top_op = top(&ops)->value.sign;
								Token* op;
								if (get_op_level(sign) <= get_op_level(top_op)) {
									op = pop(&ops);
									push(&post, *op);
								} else {
									break;
								}
							}
							push(&ops, current);
							break;
						}
					}
					default:
						break;
				}
				break;
			}
			default:	//unexpected case
				error("find unexpected token");
				break;
		}
		location++;
	} 
	while (ops.size != 0) {
		Token* op = pop(&ops);
		push(&post, *op);
	}
	free_stack(&ops);
	// 转换的后缀里的元素是相反的，所以需要反转栈中数据
	inverse(&post);
	// 计算后缀表达式
	Stack tmp;
	init_stack(&tmp);
	while (post.size != 0) {
		int tag = top(&post)->tag;
		switch (tag) {
			case INT:
			case FLOAT:
				push(&tmp, *pop(&post));
				break;
			case SIGN: {
				Token t;
				int sign = pop(&post)->value.sign;
				switch (sign) {
					case '+':
					case '-':
					case '*':
					case '/': {
						Token* value2 = pop(&tmp);
						Token* value1 = pop(&tmp);
						cal_token(value1, value2, sign, &t);
						push(&tmp, t);
						break;
					}
				}
				break;
			}
			default:
				error("stack error");
				break;
		}
	}
	if (tmp.size != 1) {
		printf("%d\n", tmp.size);
		error("bad expr");
	}
	Token* cal_result = pop(&tmp);
	Token result;
	result.tag = cal_result->tag;
	result.value = cal_result->value;

	return result;
}


void error (char* msg) {	//报错
	printf("Error( line %d ): ", line_num);
	puts(msg);
	exit(0);
}







