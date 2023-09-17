#include <stdio.h>
#define INIT 0
#define EQU 1
#define BGR 2
#define BGE 3
#define LIT 4
#define LTE 5
#define NEQ 6

void print_result(int num, int kind){
	switch(kind){
	case INIT:
		if(num != 0) {
			printf("<other,%d>", num);
		}
		break;
	case EQU:
		printf("<relop,=>");
		break;
	case BGR:
		printf("<relop,>>");
		break;
	case BGE:
		printf("<relop,>=>");
		break;
	case LIT:
		printf("<relop,<>");
		break;
	case LTE:
		printf("<relop,<=>");
		break;
	case NEQ:
		printf("<relop,<>>");
		break;
	}
	return;
}
int main()
{
	printf("Enter labLexer 1 ...\n");
	int unit_length = 0;
	int state = INIT;
	char ch = getchar();
	int do_flag = 1;
	while(do_flag){
		switch(state){
			case INIT:{
				if(ch == '\n'){
					print_result(unit_length, INIT);
					do_flag = 0;
					break;
				}
				else if(ch == '\r'){
					if((ch = getchar()) == '\n'){
						print_result(unit_length, INIT);
						break;
					}
					else unit_length++;
				}
				if(ch == '<') {
					state = LIT;
					print_result(unit_length, INIT);
					unit_length = 0;
				}
				else if(ch == '=') {
					state = EQU;
					print_result(unit_length, INIT);
					unit_length = 0;
				}
				else if(ch == '>') {
					state = BGR;
					print_result(unit_length, INIT);
					unit_length = 0;
				}
				else {
					state = INIT;
					unit_length++;
				}
				ch = getchar();
				break;
			}
			case BGR:{
				if(ch == '=') state = BGE;
				// only >
				else {
					state = INIT;
					print_result(0, BGR);
				}
				break;
			}
			case BGE:{
				print_result(0, BGE);
				state = INIT;
				ch = getchar();
				break;
			}
			case LIT:{
				if(ch == '>') state = NEQ;
				else if(ch == '=') state = LTE;
				// only <
				else{
					state = INIT;
					print_result(0, LIT);
				}
				break;
			}
			case LTE:{
				print_result(0, LTE);
				state = INIT;
				ch = getchar();
				break;
			}
			case NEQ:{
				print_result(0, NEQ);
				state = INIT;
				ch = getchar();
				break;
			}
		}
	}
	
}
