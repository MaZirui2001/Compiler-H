#include "stdio.h"
#include "pl0.h"
#ifndef LEXERGEN
void getch(){
	if(ll == 0){
		kk = 0;
		while(1){
			ch = getchar();
			line[ll++] = ch;
			if(ch == '\n') break;
		}
	}
	sym = line[kk++];
}
void getsym(){
	getch();
	switch(sym){
	case '<':{
		if(cc != 0) printf("(other,%ld)", cc);
		cc = 0;
		getch();
		if(sym == '=') sym = leq;
		else if(sym == '>') sym = neq;
		else {
			sym = lss;
			kk--;
		}
		break;
	}
	case '>':{
		if(cc != 0) printf("(other,%ld)", cc);
		cc = 0;
		getch();
		if(sym == '=') sym = geq;
		else {
			kk--;
			sym = gtr;
		}
		break;
	}
	case '=':{
		if(cc != 0) printf("(other,%ld)", cc);
		cc = 0;
		sym = eql;
		break;
	}
	case '\n':{
		if(cc != 0) printf("(other,%ld)", cc);
		cc = 0;
		sym = '\n';
		break;
	}
	case '\r':{
		getch();
		if(sym != '\n') {
			kk--;
			cc++;
			sym = nul;
		}
		else {
			if(cc != 0) printf("(other,%ld)", cc);
			cc = 0;
			sym = '\n';
		}
		break;
	}
	default: {
		cc++;
		sym = nul;
		break;
	}
	}
}
#endif
int main()
{
	printf("Enter labLexer 2...\n");
	getsym();
	int end_flag = 0;
	while(1){
		switch(sym){
		case lss: {
			printf("(relop,<)");
			break;
		}
		case leq:{
			printf("(relop,<=)");
			break;
		}
		case gtr:{
			printf("(relop,>)");
			break;
		}
		case geq:{
			printf("(relop,>=)");
			break;
		}
		case eql:{
			printf("(relop,=)");
			break;
		}
		case neq:{
			printf("(relop,<>)");
			break;
		}
		case '\n':{
			end_flag = 1;
			break;
		}
		}
		if(end_flag) break;
		getsym();
	}
	return 0;
}
