# PW 7 report

## 实验要求 

* 手工编写`assign_hand.ll`、`func_hand.ll`、`if_hand.ll`、`while_hand.ll`和`io_hand.ll`文件，以实现与SysYF程序相同的逻辑功能。
* 用SysYF IR应用编程接口，编写自己的 `assign_gen.cpp`、`func_gen.cpp`、`if_gen.cpp`、`while_gen.cpp`和`io_gen.cpp`程序，以生成与第1关的五个sy程序相同逻辑功能的ll文件。并设计快速排序程序，使用SysYF IR接口写出对应的C++文件
* 仿照`include/IR/gcd_generator.hpp`完成使用快速排序的`LLVM IR`的实现，并在`main.cpp`中调用该实现获得`IR Module`并执行，补全输入输出函数以及计时函数在`LLVM IR Module`中的声明

## **问题回答**

### 1-1 while语句对应的LLVM IR的代码布局特点

我使用llvm-15生成的while_test的语句如下：

```c
@a = dso_local global i32 0, align 4

define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 0, i32* %2, align 4
  store i32 3, i32* @a, align 4
  br label %3									  ; 进入while condition块

3:                                                ; preds = %11, %0
  %4 = load i32, i32* @a, align 4
  %5 = icmp sgt i32 %4, 0
  br i1 %5, label %6, label %9  				  ; 短路计算，通过a的比较结果来确定是否比较b和0

6:                                                ; preds = %3
  %7 = load i32, i32* %2, align 4
  %8 = icmp slt i32 %7, 10
  br label %9									  ; 跳转至对两个比较结果综合的块

9:                                                ; preds = %6, %3
  %10 = phi i1 [ false, %3 ], [ %8, %6 ]
  br i1 %10, label %11, label %17				  ; 真正的分支：决定了是否进入循环

11:                                               ; preds = %9
  %12 = load i32, i32* %2, align 4
  %13 = load i32, i32* @a, align 4
  %14 = add nsw i32 %12, %13
  store i32 %14, i32* %2, align 4
  %15 = load i32, i32* @a, align 4
  %16 = sub nsw i32 %15, 1
  store i32 %16, i32* @a, align 4
  br label %3									  ; 跳转回condition块，来判断循环是否继续进行

17:                                               ; preds = %9
  %18 = load i32, i32* %1, align 4
  ret i32 %18
}
```

和源代码对比不难看出一些while循环中LLVM IR代码布局特点：

* while采用了三块进行布局：条件代码块（可能有若干子块），true分支和false分支

* 这里的条件代码块由3个字块构成：比较a和0、比较b和10、对两个结果作综合。如果将b < 10删去，那么这三个部分将合为一个块，如图：

  ```c++
  3:                                                ; preds = %6, %0
    %4 = load i32, i32* @a, align 4
    %5 = icmp sgt i32 %4, 0
    br i1 %5, label %6, label %12
  ```

* 在代码中我们可以看到：

  * 标号3：比较a和0

  * 标号6：比较b和10
  * 标号9：对两个比较结果进行综合，决定是否进入循环
  * 标号11：while的true分支
  * 标号17：while的false分支

  **几个br的意义已经标在代码中！**

### 1.2 函数调用语句对应的LLVM IR的代码特点

观察func_test生成代码：

```c++
; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @add(i32 %0, i32 %1) #0 {
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  store i32 %0, i32* %3, align 4
  store i32 %1, i32* %4, align 4
  %5 = load i32, i32* %3, align 4
  %6 = load i32, i32* %4, align 4
  %7 = add nsw i32 %5, %6
  ret i32 %7
}

; Function Attrs: noinline nounwind optnone uwtable
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  store i32 3, i32* %2, align 4
  store i32 5, i32* %3, align 4
  %4 = load i32, i32* %3, align 4
  %5 = load i32, i32* %2, align 4
  %6 = load i32, i32* %2, align 4
  %7 = load i32, i32* %2, align 4
  %8 = add nsw i32 %6, %7
  %9 = call i32 @add(i32 %5, i32 %8)
  %10 = add nsw i32 %4, %9
  ret i32 %10
}
```

* 在调用方，调用者会先使用几条指令来算出所有参数的值，之后使用一个call来调用。call指令首先需要一个返回值类型（这里是i32），其次对每一个传递的参数，都是用“类型 数值”的方法进行传递
* 在被调用方，被调用函数会首先为所有的参数分配空间并存储，之后再加载它们进行运算



### 2.1  请给出`SysYFIR.md`中提到的两种getelementptr用法的区别, 并解释原因

第一种用来获取数组元素所在地址，第二种用来获取指针地址。

- 获取数组元素地址时，用第一个偏移0得到%1指向数组的第一个，用第二个偏移0得到首元素向后偏移量，返回此地址
- 对于第二种用法，第一个偏移0计算得到%1所指的变量，并返回该变量的地址

指令用第一个%0来计算变量的宽度，在`Type *GetElementPtrInst::get_element_type`方法中可以发现，其返回的是指针所指的变量类型，而不是指针本身类型，这样做可以方便地址的计算。



### 3.1 `printASTUnit()`方法包含的阶段

这个方法主要包含以下几个阶段

* Pipeline：判断编译工作并构造Action实例列表
* Bind：为Action实例分配特定的工具
* Translate：构建Command实例
* Execute：调用相应工具执行任务

### 3.2 简述src/driver.cpp中FrontendCodeGen()方法的功能。

从main.cpp的74行可以看出，这个方法生成了后面编译所需的module，它的本质就是通过代码的结构生成对应的代码，并将其输出



## 实验设计

### Task1

* assign_hand：主要是数组的运算，这里要灵活使用getelemntptr命令，来计算出需要赋值的地址，如：

  ```c++
   %3 = getelementptr inbounds [2 x i32], [2 x i32]* %2, i32 0, i32 0  ; %3 = a + 0
  ```

* func_hand：这里主要是对函数调用的设计。对调用者：

  ```c++
  %3 = load i32, i32* %1, align 4     ; %3 = a
  %4 = add nsw i32 %3, %3             ; %4 = a + a
  %5 = load i32, i32* %2, align 4     ; %5 = a
  %6 = call i32 @add(i32 %3, i32 %4)  ; %6 = add(a, a+a)
  ```

  先加载出所需实参，调用call指令来完成函数调用。而对于被调用方：

  ```c++
  %3 = alloca i32, align 4            ; %3 = &a
  %4 = alloca i32, align 4            ; %4 = &b
  store i32 %0, i32* %3, align 4      ; a = %0
  store i32 %1, i32* %4, align 4      ; b = %1
  ```

  将传入的形参进行存储，之后再执行被调用函数体。

* if_hand：使用br分支指令来完成if，但对于这里的或运算，需要额外加入一个条件判断块：

  ```c++
  %3 = icmp sgt i32 %2, 0             ; compare a with 0
  br i1 %3, label %6 , label %4       ; if a > 0, goto exec, else, goto compare a with 6
  4: 
  %5 = icmp slt i32 %2, 6             ; compare a with 6
  br i1 %5, label %6, label %7        ; if a < 6, goto exec, else skip exec
  ```

  这里6处是if的真分支，7是if的假分支，通过短路计算，就可以实现这样的或运算

* while_hand：使用condition块、true块和false块进行实现，不过这里依然有和if中相同的问题，就是需要对条件表达式进行短路计算，我依然采用了添加额外条件判断块的方法来解决：

  ```c++
  3: 
      %4 = load i32, i32* @a, align 4     ; %4 = a
      %5 = load i32, i32* %2, align 4     ; %5 = b
      %6 = icmp sgt i32 %4, 0             ; compare a with 0
      br i1 %6, label %7, label %12       ; if a > 0, go to compare, else, gskip exec 
  7:
      %8 = icmp slt i32 %5, 10            ; compare b with 10
      br i1 %8, label %9, label %12       ; if b < 10, go in exec, else, skip exec
  9:                                      ; exec
      %10 = add nsw i32 %4, %5            ; %10 = a + b
      store i32 %10, i32* %2, align 4     ; b = a + b
      %11 = sub nsw i32 %4, 1             ; %11 = a - 1
      store i32 %11, i32* @a, align 4     ; a = a - 1
      br label %3                         ; go back to while
  12:
      %13 = load i32, i32* %1, align 4    ; %13 = 0
      ret i32 %13                         ; return 0
  ```

  其中，3对a>0进行判断，7对b<10进行判断，9为true块，12为false块

* io_hand：在调用函数的基础上，这里更需要注意对外部函数调用时的声明：

  ```c++
  declare i32 @getint() #1
  
  declare void @putint(i32) #1
  
  declare i32 @getfarray(float*) #1
  
  declare void @putfarray(i32, float*) #1
  ```

### Task 2

在第二关中，我们只需要熟悉SysYF IR应用编程接口，调用其中的函数进行编程即可。在这里，我选择其中重点部分进行说明

* 概述：所有的c++代码都必须有以下两个对象（这里以assign_gen为例）：

  ```c++
  auto module = new Module("assign code");
  auto builder = new IRStmtBuilder(nullptr,module);
  ```

* assign_gen：这里重点是main函数的创建以及数组地址的计算。对于main函数，我们需要使用FunctionType的方法get来创建函数类型对象，用Function的方法create来创建函数对象：

  ```c++
  auto mainFunTy = FunctionType::get(Int32Type,{});
  auto mainFun = Function::create(mainFunTy, "main", module)
  ```

  计算数组地址时，类似于task1的getelemntptr指令，这里使用create_gep来实现：

  ```c++
  auto a0_gep = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(0)});
  ```

* func_gen：这里需要考虑函数调用和函数参数传递，对于函数调用，我们只需要使用create_call完成调用即可：

  ```c++
  a_load = builder->create_load(a_alloca);
  iadd = builder->create_iadd(a_load, a_load);
  auto add = builder->create_call(add_fun, {a_load, iadd});
  ```

  对于参数的传递，在被调用方，我们采用一个vector来存储参数：

  ```c++
  for (auto arg = add_fun->arg_begin(); arg != add_fun->arg_end(); arg++) {
      args.push_back(*arg);
  }
  builder->create_store(args[0], a_alloca);
  builder->create_store(args[1], b_alloca);
  ```

* if_gen：这里需要考虑如何使用icmp和br进行分支。在分支时，我们需要创建几个block，并确定指令插入的位置：

  ```c++
  auto trueBB = BasicBlock::create(module, "trueBB_if", main_fun);    
  auto falseBB = BasicBlock::create(module, "falseBB_if", main_fun);  
  ```

  使用icmp和br完成跳转：

  ```c++
  auto a_load = builder->create_load(a);
  auto icmp = builder->create_icmp_gt(a_load, CONST_INT(0));
  builder->create_cond_br(icmp, trueBB, falseBB); 
  ```

* while_gen：和if类似，这里需要多出一个condBB，用来作条件的判断。同时，为了完成短路计算，也需要扩展更多的condBB块。实现如下：

  ```c++
  // condBB
  builder->set_insert_point(condBB);
  auto a_load = builder->create_load(a);
  auto icmp = builder->create_icmp_gt(a_load, CONST_INT(0));
  builder->create_cond_br(icmp, trueBBif, falseBB);
  // trueBB_if
  builder->set_insert_point(trueBBif);
  auto b_load = builder->create_load(b_alloca);
  icmp = builder->create_icmp_lt(b_load, CONST_INT(10));
  builder->create_cond_br(icmp, trueBB, falseBB);
  ```

  对于while体内块，应当在最后加上一条无条件跳转到conBB的br语句：

  ```c++
  builder->create_br(condBB);
  ```

* io_gen：基本与func_gen无异，但需要在之前加上一些函数的声明：

  ```c++
  auto getint = scope->find("getint", true);
  auto putint = scope->find("putint", true);
  auto getfarray = scope->find("getfarray", true);
  auto putfarray = scope->find("putfarray", true);
  ```

* qsort_gen：我的qsort代码参考了算法导论第3版：

  ```c++
  void qsort(int p, int r){
      int q;
      if(p < r){
          q = partition(p, r);
          qsort(p, q - 1);
          qsort(q + 1, r);
      }
      return;
  }
  ```

  首先我们需要一个全局数组来保存需要排序的数据：

  ```c++
  auto *arraytype_a = ArrayType::get(Int32Type, 100000);
  auto zero_initializer = ConstantZero::get(Int32Type, module_ptr);
  auto a = GlobalVariable::create("a", module_ptr, arraytype_a, false, zero_initializer);
  ```

  其余部分比较复杂的，就是partition部分中的循环和分支嵌套的情况。这一部分只是单纯的工作量问题，具体请参考我的代码实现（其中已经加入了注释）。和之前几个gen不同的地方主要在于递归函数，不过在这里递归函数也可以直接用函数调用，不必过多考虑其他问题（这得益于，每次调用函数后都会把参数保存起来）：

  ```c++
  // qsort(a, p, q - 1);
  auto q_minus1 = stmt_builder->create_isub(q_load, CONST_INT(1));
  stmt_builder->create_call(qsort_fun, {p_load, q_minus1});
  
  // qsort(a, q + 1, r);
  auto q_add1 = stmt_builder->create_iadd(q_load, CONST_INT(1));
  stmt_builder->create_call(qsort_fun, {q_add1, r_load});
  ```

### Task 3

第三关中主要的是qs_generator的设计，和补全输入输出函数以及计时函数在`LLVM IR Module`中的声明。

* qs_generator：这里和Task2中的区别在于换了一种表述的方法，函数名作了改变，我们只需要熟悉新的函数调用即可。这里值得一提的是数组的传递。之前已经在io中完成了对以数组为参数的函数的调用，但其实现需要特别注意一点：

  ```c++
  auto arr = args[0];
  ```

  我们不应该为传入的数组参数分配一个数组的空间，它只是一个指针！因此我们直接取出这个数组的参数，就可以顺利完成后面全部的计算。

  除此以外，对于外部函数的调用，应当如下实现：

  ```c++
  builder.CreateCall(runtime->put_char_func, {CONST_INT(' ')});
  ```

* 补全输入输出函数以及计时函数在`LLVM IR Module`中的声明;

  这里需要注意，函数名不可以乱取，必须按照io.cpp中的函数进行实现。由于已经给出示例，我们需要思考和示例函数类型不同的函数应当如何实现，例如getfarray，我们需要考虑它传入参数的类型：

  ```c++
  get_float_array_func = Function::Create(FunctionType::get(Type::getInt32Ty(module->getContext()), {Type::getFloatPtrTy(module->getContext())}, false),GlobalValue::LinkageTypes::ExternalLinkage,"getfarray", module); 
  ```

  可以看出，输入的类型应该是一个浮点数指针。

  下面的runtime_info，只需要按照示例制作即可，这里不作过多说明



## 实验难点及解决方案

* getelementptr的使用：这条指令应该是比较难以理解的部分。最初我对它的理解并不透彻，导致了各种各样的问题。后来通过查阅资料，我得知了这条指令只是用来计算数组和指针地址的。对于数组的计算，需要在第三个参数中加入`i32 0`,而指针计算是不需要的。
* 对block的理解：一个block就可以看做一对“大括号”，和我们的直观感知不太相同，每一个block的最后一条指令必须是跳转到另一个block的分支指令——但这又很符合程序设计的思维。最开始我不知道这一点，导致生成的ll文件代码乱拍，还引用了未定义的位置。后来通过对示例代码的理解，解决了这个问题
* 类型的理解：最开始我认为，传入函数的应该是一个数组，因此在Task3中，为第一个参数分配了一个数组的空间，并且使用了违规的CreateGEP来计算。不过经过我的“cout”手段，找到了这里的问题，并理解了传入的参数只是一个指针，不应该当做数组来处理的事实。
* runtime函数的补齐：最开始没有意识到函数名必须和io.cpp中的相同，导致编译运行的时候一直报段错误，一直以为是函数传参导致数组越界，但经过仔细的进行cout，发现原来是并没有找到这个getfarray函数，根本原因是没有定义好getfarray。经过修改成功运行。

## 实验总结

通过本次实验，我有了如下收获：

* 了解了SysYF IR，并可以编写对应的ll文件用以执行
* 了解了LLVM和Clang的基本原理，并可以调用LLVM编程接口进行编程

## 实验反馈

* 平台上“在此处打开终端”，可不可以改成真的“在此处打开终端”呀！
* 这次实验的文档有一些不够详细的地方，比如对getelementptr的参数，阐释的不够详细，容易产生一些误解

