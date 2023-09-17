# Question 1-1

观察main.cpp可以看出，main函数首先负责处理命令行参数：

* -h：打印帮助信息
* -p：将bool型trace_parsing变量设为1，跟踪语法分析的细节（在demoDriver中的set_debug_level中体现）
* -s：将bool型trace_scaning变量设为1，跟踪扫描的细节（在demoDriver中lexer成员的set_debug中体现）
* -emit-ast：设置好print_ast变量，准备打印构建的语法树

之后，main函数调用demoDriver的parse方法，创建了一棵语法树，并将其根节点指针返回。如果之前要求打印语法树，则root指针通过调用accept函数，来打印语法树

在parse方法中，首先调用scan_begin函数，这个函数再通过调用demoFlexLexer类的一个对象lex的switch_streams方法进行词法分析，如果不能打开文件则输出错误信息并退出程序。之后返回scan_begin函数，调用demoParser中的parse方法进行语法分析。

语法分析完毕后，调用scan_end关闭文件，并返回主函数，整个过程结束。

# Question 1-2

如果要解决这个问题，我们应当考虑，main是可以作为一个变量名出现的，因此不能将main作为一个关键字来处理。

因为demo中只可以有一个函数定义，所以我们可以在语法分析时直接对这个函数名进行判断，如果不是main，那么输出报错信息并退出分析程序。

找到语法分析部分对应函数定义的代码：

```c++
FuncDef: VOID IDENTIFIER LPARENTHESE RPARENTHESE Block{
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type = SyntaxTree::Type::VOID;
    $$->name = $2;
    $$->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>($5);
    $$->loc = @$;
  }
  ;
```

修改name的逻辑：

```c++
FuncDef: VOID IDENTIFIER LPARENTHESE RPARENTHESE Block{
    $$ = new SyntaxTree::FuncDef();
    $$->ret_type = SyntaxTree::Type::VOID;
    $$->name = $2;
    if($2 != "main"){
        std::cout << "The function name is not 'main'!" << std::endl;
        exit(0);
    }
    $$->body = SyntaxTree::Ptr<SyntaxTree::BlockStmt>($5);
    $$->loc = @$;
  }
  ;
```

如此一来，就可以规定demo中的唯一函数必须为main函数了。