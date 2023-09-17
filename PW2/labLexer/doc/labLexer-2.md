# labLexer-2 report

## 实验内容

* 使用flex，构建词法分析器

## 问题分析

* 首先，我们应当对.lex文件的语法进行了解，其语法基本与正则表达式相同。同时，利用pl0.h中的各类变量，就可以实现字符串的处理
* 经过观察示例可以看出，这个词法分析器会把读入的字符流全部存储在line这个数组中，使用指针进行访问时可以非常便捷地实现回退操作
* 再观察pl0.h中的变量，cc显然是一个用来计数的变量。因此，在自己实现词法分析器的时候，可以操作这个变量来完成对连续非关系运算符的计数

## 实验设计

* 如果使用flex生成词法分析器，则语法单元应该写作：

  ```
  
  
  %{
  #include "pl0.h"
  #include <stdio.h>
  %}
  
  %%
  
  "<"         {
      if(cc > 0) printf("(other,%ld)", cc);
      cc = 0;
      return(lss);}
  "<="         {
      if(cc > 0) printf("(other,%ld)", cc);
      cc = 0;
      return(leq);}
  ">"         {
      if(cc > 0) printf("(other,%ld)", cc);
      cc = 0;
      return(gtr);}
  ">="         {
      if(cc > 0) printf("(other,%ld)", cc);
      cc = 0;
      return(geq);}
  "="         {
      if(cc > 0) printf("(other,%ld)", cc);
      cc = 0;
      return(eql);}
  "<>"         {
      if(cc > 0) printf("(other,%ld)", cc);
      cc = 0;
      return(neq);}
  [\n|\r\n]  {
      if(cc > 0) printf("(other,%ld)", cc);
      cc = 0;
      return('\n');}
  .          {cc++;}
  %%
  void getsym()
  {
  	sym = yylex();
  }
  
  ```

  这里定义了对各类字符应当采取的操作。之后，getsym事实上是一层包装，这样可以方便我们进行条件编译时，简单地决定使用与不使用flex生成的词法分析器。

* 自行实现的词法分析器：

  ```c++
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
  ```

  getch用于在缓冲区中获得一个字符，而getsym则会修改pl0.h中sym的值，记录最后一个读入的标识符。这两者的行为契合了flex生成的词法分析器中yylex的行为，这样可以使主函数进行调用时，不管是否条件成立，都可以找到对应的getsym。

## 遇到的问题

* **如何表示“其他字符”**

  在lex文件中，其他字符使用了.来进行表示，这和lex文件的性质有关，它的行为类似switch语句，会从上到下逐个检查是否符合条件。因此，只要把.写到最后，就可以成功用来表示“其他字符”
  
* gcc无法编译：

  若gcc版本大于等于11，则对于头文件中定义的变量，当该头文件被多次引用时，将报出重复定义的错误。在本地我将gcc版本退回9，成功进行了编译