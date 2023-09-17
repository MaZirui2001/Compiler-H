# labLexer-1 report

## 实验内容

* 手写一个可以识别关系运算符的词法分析器

## 问题分析

* 根据教材中给出的状态转换图，进行对应的字符处理，并进行对应的状态转换
* 这里的难点主要在于“读到不是关系运算符就回退”的操作。由于getchar()的读入是不可逆的，因此我们需要在不同状态下分别考虑是否要进行getchar()；

## 实验设计

采用了教材中给出的状态转换图，通过getchar()来进行字符流的读入：

状态转移部分代码如下：

```c++
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
		case EQU:{
			print_result(0, EQU);
			state = INIT;
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
```

* 状态转换基本遵循了教材中的转换图。值得注意的是，这种转换方法能且仅能在INIT状态下读到行位符号，因此只需在INIT状态下进行判断即可。
* 这里特别对\r\n和\n进行了处理，如果\r后紧接\n，那么会直接终止词法分析,否则认为\r是一个普通字符。

## 遇到的问题

* **对于“指针回退”的处理**

  在处理小于号时，如果下一个字符不是大于号或等于号，那么需要字符流指针回退，但这个指针事实上并不存在。在这里我采用了一种简单但具有局限性的做法：在每次循环时，我们并不都进行getchar()，而是在一个完整的字符被识别出来时才进行getchar()，这样避免了指针回退的问题。但是，一旦存在一个长度大于等于3的关系运算符，这种方法就会使中间的字符被丢弃，因此存在一定的局限性