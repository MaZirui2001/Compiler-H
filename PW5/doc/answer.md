# answer

## 问题1-1

### 各个节点类内部结构

* 存放该节点信息的数据集合，这些信息记录了节点类型（例如VarDef中的btype），或者是节点的各种属性（例如FuncParam中的name）
* 一个指向其他节点的指针列表或几个指向其他节点的指针（例如IfStmt中的 if_statement和 else_statement，以及FuncParam中的array_index）

* 一个accept接口函数

### visit方法实现的特点

* 基本上都是对节点中的数据进行简单处理后打印，例如添加了大括号或对打印格式作对齐等调整
* 循环调用被visit对象指向的其他节点的accept方法，来递归访问这些关联节点（例如通过FuncDef中body成员的accept接口来打印这个函数中函数体的内容）

## 问题3-1

### 遇到的困难与解决方案

* 对各模块牵一发而动全身，不容易辨别出完成哪些部分可以完成task2

  我对所有模块可能报出的错误进行了梳理，阅读task2的文档发现，其实这里基本就要完成大部分内容，只不过对参数类型错误等错误不需要苛责。

* Expr_int的理解

  起初我对访问者模式的递归调用理解不深刻，故而对Expr_int这个变量的理解也不深刻（特别是在FuncCall的visit中）。通过仔细研读代码我发现，事实上对于这种递归调用，只需要在本层访问中设置好本层的数据类型，就可以在上层调用中使用Expr_int来完成很多判断。这也是后来我加入了Expr_type这个变量的重要启示。

* 什么时候设置类型？什么时候增加符号表项？

  这两个问题困扰了我许久。当我决定加入Expr_type这个变量后，我对这个变量的使用在很长一段时间内没有摸索清楚。后来我领悟了递归调用，我明白了Expr_type和Expr_int事实上是两个信息传递单元，它们最应当在函数调用结束前设置。

  符号表项我选择了vector来维护，这里就出现了一个问题：何时分配新的空间。经过对书7.2.2节进行研读，我决定为每一层作用域都分配一个符号表。这时，例如BlockStmt这种节点，检查时需要将作用域+1，那么这时就需要为符号表push_back一下，在内层检查结束后，我们再将符号表结尾这个单元删除。

### 实验难点和考察倾向

* 作用域的判断和记录
* 符号表数据结构的设计
* 每一类节点visit函数的设计
* 主要考察我们对于类型检查的理解以及对代码的阅读理解能力

### 整体实现思路和实现亮点

* 实现思路

  1. 单点入手，辐射全局。

     受到了取模运算检查的启示，我从Vardef对应的visit入手，发现我们需要维护一个符号表。而对于符号表，我又查询了课本，有了一定的启发，于是完成了对符号的记录。之后我看到了作用域，于是进一步先完成了BlockStmt，并思考了BlockStmt会关联哪些节点类型，一步步深入，完成了所有的设计。

  2. 细节思考，完善设计

     在基本完成了所有模块的编写后，我考虑到符号表查询较为复杂，因此将符号表从map拓展为class，提供了各项方法来插入查询符号表。为了简化符号表的索引，我也将符号表从数组变为vector，并为符号表项中的属性中的该项类型设计了enum类，用以较好的进行标识。

* 设计亮点

  1. 符号表的设计：

     符号表我使用了unordered_map进行实现。unordered_map使用哈希表实现，效率较高，且由于符号表的性质，我们只有查找需求，没有按序访问需求。实现如下：
     ```c++
     class SymbolTable
     {
     
     private:
         std::unordered_map<std::string, Attributes> sym_table;
     
     public:
         SymbolTable() {}
     
         // search the location of name
         Attributes search(std::string name)
         {
             return sym_table[name];
         }
     
         // add item to table
         void add_item(std::string name, Attributes attr)
         {
             sym_table[name] = attr;
         }
     
         // check if an item is in table
         bool item_exist(std::string name)
         {
             return sym_table.find(name) != sym_table.end();
         }
     
         ~SymbolTable() {}
     };
     ```

     这个符号表类可以对表项进行查找、判断是否存在以及添加表项，这足以应对当前语义分析器的全部需求。

     我为符号表项建立了一个新的结构体类型——Attribute，用以记录符号表的属性：

     ```c++
     enum class AttrType
     {
         Var = 0,
         Func
     };
     
     struct Attributes
     {
         AttrType def_type;
         SyntaxTree::Type type;
         std::vector<SyntaxTree::Type> FParamList;
     };
     ```

     其中，def_type表明了这个表项是函数还是变量，而type表示了变量的类型或函数的返回值类型，FParamList是函数的参数列。

     这个符号表的设计特色是：

     * 查找效率很高
     * 操作较为简单，频繁访问时可以基本保持较高效能
     * 动态分配空间，空间利用量不大。

  2. 作用域的实现：

     我们在这里维护了两个变量：

     ```c++
     std::vector<SymbolTable> symbol_table;
     int loc_scope = 0;
     ```

     loc_scope是当前语义分析所在的作用域，而我们使用vector为每一个作用域分配一个符号表，这样可以用loc_scope进行索引，很快地查找到对应作用域的符号表。

     在visit进行时，loc_scope会逐渐变化，最典型的就是BlockStmt：

     ```c++
     void SyntaxTreeChecker::visit(BlockStmt &node)
     {
         loc_scope++;
         symbol_table.push_back(SymbolTable());
         for (auto block_stmt : node.body)
             block_stmt->accept(*this);
         symbol_table.pop_back();
         loc_scope--;
     }
     ```

## 问题3-2

* 处理变量声明时，应当先把变量加入符号表，因为变量是可以不进行初始化的。
* 处理函数声明时，应当先把函数名加入符号表，因为函数中可能存在递归调用。

## 问题3-3

修改Expr属性有以下缺点：

1. 需要对AST的构造方法进行修改，降低AST构造的性能，并增大AST的存储空间
2. 表达式的类型判断并不容易，例如C语言中，需要对两个表达式的值都进行判断后，才能判断出最终结果的表达式，这无疑增大了AST的构造开销

拆成多个类有以下缺点：

1. 在检查一个节点时，需要对每个类都调用一次accept，这无疑降低了程序运行的效率，也增大了函数调用的开销
2. 每个类处理一种语义错误，这反而会增大各类间协同处理的难度，对于“哪个类先处理哪个类后处理”的问题又会出现，使得设计更加复杂

## 问题3-4

为每一种错误分配一个优先级，之后维护一个优先级队列，当遇到错误时不要报错后直接退出，而是将错误保存在这个优先级队列中。当所有错误都识别完毕后，只需报出队头元素的错误即可。

