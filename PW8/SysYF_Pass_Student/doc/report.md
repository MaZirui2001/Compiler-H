# 实验报告

## 必做部分

### 支配树

#### B1-1

证明：在这个流图中，我们不妨假设存在一条由 $ENTER$ 出发，经过 $x$ 但还未经过 $y$ 的路径的 $P_1$，否则我们可以沿着 $P_1$ 逆流而上，逐个消除遇到的 $x$ 或 $y$ 直至 $P_1$ 满足要求。

下面我们用反证法证明推论一：此时对于任何一条从 $x$ 出发并经过 $b$ 的路径 $P$，其在第一次经过 $b$ 前均曾经过 $y$.

事实上，假设存在路径 $P_2$，其直接由 $x$ 出发，未曾经过 $y$ 就到达 $b$，此时存在路径 $P_1+P_2$，其连接 $ENTER$ 与 $b$ 但不经过 $y$，也就是此时 $y$ 不支配 $b$，矛盾！

我们由推论一可以得到推论二：对于任何一条从 $ENTER$ 出发且经过 $b$ 的路径，其一定也经过 $x$ 和 $y$，我们指出其在第一次经过 $b$ 前，上一个经过的 $x$ 和 $y$ 中的点一定是 $y$，否则我们立即找到一条否定推论一的路径。

由推论二我们可以得到，一定存在某条从 $y$ 出发，经过 $b$ 但还未经过 $x$ 的路径 $P_3$，由此我们可以得到推论三：此时不存在任何一条路径，其从 $ENTER$ 出发且在经过 $y$ 时没有经过 $x$，否则，设存在这样的路径 $P_4$，于是 $P_4+P_3$ 是一条从 $ENTER$ 出发且经过 $b$ 时没有经过 $x$ 的路径，这与 $x$ 支配 $b$ 矛盾。

推论三等价于每一条从 $ENTER$ 出发且经过 $y$ 的路径必经过 $x$，也就是 $x$ 支配 $y$，至此我们完成了对原命题的证明。

#### B1-2

答：不需要。这是因为数据流的迭代收敛结果与基本块被运算的顺序无关；但是按照后序遍历的逆序进行循环操作，可以有效减少迭代循环的次数。

#### B1-3

答：这个问题实际上非常 `tricky`，本人认为实际上问题的答案与这一份伪代码究竟如何实现直接相关：

1. 之所以本人认为可以回答 “必须使用后序遍历的逆序”，是因为如果不这么做，有可能出现某一基本块的全部前驱都未计算的情况，反应在原代码中，这种情况是原代码所没有考虑的。事实上，原代码也确实不必考虑这一情况，因为以后序遍历的逆序遍历所有基本块时，可以保证每一个基本块都至少有一个前驱已经被计算；
2. 之所以本人认为也可以回答 “不必使用后序遍历的逆序”，是因为如果这样做的话，只需要在伪代码的实现中考虑如下情况：使得一个基本块在所有前驱均未被计算的情况下继续标注为未计算即可。这样做的话，就可以用任意顺序遍历所有的基本块了。

#### B1-4

答：两个问题的答案分别如下所示：

1. `intersection` 的作用是寻找两个基本块在支配树上的最近公共祖先，进而获取支配这两个基本块的所有基本块，换言之就是求得了两个基本块的支配集合的交集；
2. 不能改为大于号。理由是这个算法的正确性的重要保证是：由于基本块编号是由后序遍历标号的，所以在支配树上一定是编号大的基本块支配编号小的基本块。正因如此，在支配树数组上寻找公共祖先的安全方式，只能是移动现在对应编号较小的那个指针。从数值的角度解释，移动将使得指针对应的编号变大（除非移动到根节点），如果移动的一直是偏大的指针，那么两指针永远不可能相遇。

#### B1-5

答：我分为时间优点，空间优点和其它优点来阐述这一算法的优点：

1. 时间优点：首先，课本上的算法没有采用 “后序遍历的逆序” 这一基本块访问策略；其次，由于采用了在支配树上寻找结点公共祖先的算法，大大节约了求集合交集以及构造结果集合的时间；最后，从支配树上读取支配集合是极为方便的，这种读取结果的速度和在散列表中遍历所有元素相当；
2. 空间优点：相较于课本每个基本块 “都需要保存支配集合” 的方案，用支配树的方法大大节约了支配集合的存储空间，所有基本块的支配信息都共同记录于支配树中；
3. 其它优点：支配树的设计可以方便地为其它数据流分析问题提供高效方案，例如在 `Dominance Frontiers` 的计算中，可以直接使用支配树的信息以辅助高效实现。

#### B1-6

答：两个问题的答案分别如下所示：

1. 确定一个基本快是否是 `EXIT` 的方法是检查这个基本块的最后一条语句是否是 `ret` 语句；
2. 首先很难定义什么是流图的 ”最后一个基本块“，其次由于包含 `ret` 语句的块不一定在代码的最后，也不一定在出现在某种遍历的最后，所以这种简单的判断方式是不正确的。

### Mem2Reg

#### B2-1

阅读 `Mem2Reg::execute()` 函数，可知优化遍的流程如下：

遍历模块中的各个函数，对每个函数依次进行如下操作：

- 判断当前函数是否有基本块，变量初始化（清空表明左值之间关联的 `lvalue_connection`， 无交集 `no_union_set` 等）
- 执行 `insideBlockForwarding()`：对当前函数的每个基本块进行遍历，执行以下操作，进行块内优化：
  - 遍历基本块中的每一条指令，如果当前指令不只涉及局部变量，则跳过，否则分类型处理
    - 如果是 `store` 指令：检查当前指令是否已在 `forward_list` 中，如果在，则找到对应的 `rvalue`；检查当前指令的 `lvalue` 是否已在 `defined_list` 中，根据情况决定是否插入 `delete_list` 或 `defined_list`；检查当前指令的 `lvalue` 是否已在 `new_value` 中，并决定对 `new_value` 进行更新或插入新值
    - 如果是 `load` 指令：检查当前指令的 `lvalue` 是否已在`defined_list`中，如果在，则在 `new_value` 中找到新值，并将`inst-new_value`对加入 `forward_list`
  - 遍历 `forward_list`，对于每一个`inst-value`对，对该 `inst` 的 `use_list`遍历，将所有可以被替换的操作数替换为 `value`
  - 删除无用的 `load` 和 `store`
- 执行 `genPhi()`，即找到全局名字（跨多个基本块的活动变量名）集合，对每个名字，分别插入 $\phi$ 函数对应的指令；
- 执行 `valueDefineCounting()`：为了完成SSA，变量需要重新命名以彼此区分，此部分对于需要重命名的变量进行计数，并添加唯一的下标；
- 执行 `valueForwarding()`：即重命名算法的主要部分，见参考文档 《静态单赋值格式构造》中图9-12算法
  - 首先，遍历当前基本块中的每一条$\phi$指令，对应修改`value_status`（`Value`的一对多映射）
  - 对于所有的 `load` 指令涉及的`lvalue`，对所有的`use`都替换成最新的`new_value`；对于所有的 `store` 指令，对应更新 `value_status` （入栈）；删除无用指令；
  - 遍历当前基本块的所有后继基本块，处理$\phi$指令，填入相应的操作数，并继续对后继基本块递归调用`valueForwarding`，完成块间优化
  - 对于当前基本块定义的变量，依次对`value_status`中该变量对应的栈做一次弹出；
  - 删除无用指令
- 执行 `removeAlloc`：除了数组的 `alloca` 指令外，其他类型（包括 `int`, `float` , `pointer`）的 `alloca` 指令均会被删除。

#### B2-2

例子：

```
int main(){
    int a, b;
    a = 1;
    b = 1;
    while (a <= 10) {
        if (a % 2 == 0) {
            b = b + a;
            a = a + 1;
        }
        else {
            b = b - a;
            a = a + 3;
        }
    }
    // b = 1 - 1 + 4 - 5 + 8 - 9 = -2
    putint(b);
	return 0;
}
```

不开优化时产生的IR：

```
declare i32 @get_int()

declare float @get_float()

declare i32 @get_char()

declare i32 @get_int_array(i32*)

declare i32 @get_float_array(float*)

declare void @put_int(i32)

declare void @put_float(float)

declare void @put_char(i32)

declare void @put_int_array(i32, i32*)

declare void @put_float_array(i32, float*)

define i32 @main() {
label_entry:
  %op0 = alloca i32
  %op1 = alloca i32
  %op2 = alloca i32
  store i32 1, i32* %op1
  store i32 1, i32* %op2
  br label %label4
label_ret:                                                ; preds = %label15
  %op3 = load i32, i32* %op0
  ret i32 %op3
label4:                                                ; preds = %label_entry, %label29
  %op5 = load i32, i32* %op1
  %op6 = icmp sle i32 %op5, 10
  %op7 = zext i1 %op6 to i32
  %op8 = icmp ne i32 %op7, 0
  br i1 %op8, label %label9, label %label15
label9:                                                ; preds = %label4
  %op10 = load i32, i32* %op1
  %op11 = srem i32 %op10, 2
  %op12 = icmp eq i32 %op11, 0
  %op13 = zext i1 %op12 to i32
  %op14 = icmp ne i32 %op13, 0
  br i1 %op14, label %label17, label %label23
label15:                                                ; preds = %label4
  %op16 = load i32, i32* %op2
  call void @put_int(i32 %op16)
  store i32 0, i32* %op0
  br label %label_ret
label17:                                                ; preds = %label9
  %op18 = load i32, i32* %op2
  %op19 = load i32, i32* %op1
  %op20 = add i32 %op18, %op19
  store i32 %op20, i32* %op2
  %op21 = load i32, i32* %op1
  %op22 = add i32 %op21, 1
  store i32 %op22, i32* %op1
  br label %label29
label23:                                                ; preds = %label9
  %op24 = load i32, i32* %op2
  %op25 = load i32, i32* %op1
  %op26 = sub i32 %op24, %op25
  store i32 %op26, i32* %op2
  %op27 = load i32, i32* %op1
  %op28 = add i32 %op27, 3
  store i32 %op28, i32* %op1
  br label %label29
label29:                                                ; preds = %label17, %label23
  br label %label4
}

```

开启`-O`选项后产生的IR：

```
declare i32 @get_int()

declare float @get_float()

declare i32 @get_char()

declare i32 @get_int_array(i32*)

declare i32 @get_float_array(float*)

declare void @put_int(i32)

declare void @put_float(float)

declare void @put_char(i32)

declare void @put_int_array(i32, i32*)

declare void @put_float_array(i32, float*)

define i32 @main() {
label_entry:
  br label %label4
label_ret:                                                ; preds = %label15
  ret i32 0
label4:                                                ; preds = %label_entry, %label29
  %op30 = phi i32 [ 1, %label_entry ], [ %op32, %label29 ]
  %op31 = phi i32 [ 1, %label_entry ], [ %op33, %label29 ]
  %op6 = icmp sle i32 %op31, 10
  %op7 = zext i1 %op6 to i32
  %op8 = icmp ne i32 %op7, 0
  br i1 %op8, label %label9, label %label15
label9:                                                ; preds = %label4
  %op11 = srem i32 %op31, 2
  %op12 = icmp eq i32 %op11, 0
  %op13 = zext i1 %op12 to i32
  %op14 = icmp ne i32 %op13, 0
  br i1 %op14, label %label17, label %label23
label15:                                                ; preds = %label4
  call void @put_int(i32 %op30)
  br label %label_ret
label17:                                                ; preds = %label9
  %op20 = add i32 %op30, %op31
  %op22 = add i32 %op31, 1
  br label %label29
label23:                                                ; preds = %label9
  %op26 = sub i32 %op30, %op31
  %op28 = add i32 %op31, 3
  br label %label29
label29:                                                ; preds = %label17, %label23
  %op32 = phi i32 [ %op26, %label23 ], [ %op20, %label17 ]
  %op33 = phi i32 [ %op28, %label23 ], [ %op22, %label17 ]
  br label %label4
}

```

遍历模块中的各个函数，对每个函数依次进行如下操作：

- 判断当前函数是否有基本块，变量初始化（清空表明左值之间关联的 `lvalue_connection`， 无交集 `no_union_set` 等）
- 执行 `insideBlockForwarding()`：对当前函数的每个基本块进行遍历，执行以下操作，进行块内优化
- 执行 `genPhi()`，即找到全局名字（跨多个基本块的活动变量名）集合，对每个名字，分别插入 $\phi$ 函数对应的指令；

```
  // label4
  %op30 = phi i32 [ 1, %label_entry ], [ %op32, %label29 ]
  %op31 = phi i32 [ 1, %label_entry ], [ %op33, %label29 ]
  // label29
  %op32 = phi i32 [ %op26, %label23 ], [ %op20, %label17 ]
  %op33 = phi i32 [ %op28, %label23 ], [ %op22, %label17 ]
```

- 执行 `valueDefineCounting()`：为了完成SSA，变量需要重新命名以彼此区分，此部分对于需要重命名的变量进行计数，并添加唯一的下标；
- 执行 `valueForwarding()`：即重命名算法的主要部分，见参考文档 《静态单赋值格式构造》中图9-12算法
  - 首先，遍历当前基本块中的每一条$\phi$指令，对应修改`value_status`（`Value`的一对多映射）
  - 对于所有的 `load` 指令涉及的`lvalue`，对所有的`use`都替换成最新的`new_value`；对于所有的 `store` 指令，对应更新 `value_status` （入栈）；删除无用指令；
  - 遍历当前基本块的所有后继基本块，处理$\phi$指令，填入相应的操作数，并继续对后继基本块递归调用`valueForwarding`，完成块间优化
  - 对于当前基本块定义的变量，依次对`value_status`中该变量对应的栈做一次弹出；
  - 删除无用指令
- 执行 `removeAlloc`：除了数组的 `alloca` 指令外，其他类型（包括 `int`, `float` , `pointer`）的 `alloca` 指令均会被删除。

以 `label17` 为例，将`op1, op2`替换成 `new_value` （分别是`op30, op31`）后结果如下：

```
label17:                                                ; preds = %label9
  %op18 = load i32, i32* %op30
  %op19 = load i32, i32* %op31
  %op20 = add i32 %op18, %op19
  store i32 %op20, i32* %op30
  %op21 = load i32, i32* %op31
  %op22 = add i32 %op21, 1
  store i32 %op22, i32* %op31
  br label %label29
```

再删除无用指令后：

```
label17:                                                ; preds = %label9
  %op20 = add i32 %op30, %op31
  %op22 = add i32 %op31, 1
  br label %label29
```



问题回答：

> `Mem2Reg`可能会删除的指令类型是哪些？对哪些分配(alloca)指令会有影响？

1. 所有 `isLocalVarOp() == true` 的指令会被删除，即那些没有使用全局名字、也没有使用指向数组元素的指针的`load` 和 `store` 指令；所有 `int`，`float` 或 `pointer` 类型的 `alloca` 指令（见 `removeAlloc()` 函数）
2. 除了使用数组元素的 `alloca` 之外的所有 `alloca` 指令（见上）。

> 在基本块内前进`insideBlockForwarding`时，对`store`指令处理时为什么`rvalue`在`forward_list`中存在时，就需要将`rvalue`替换成`forward_list`映射中的`->second`值？

`rvalue` 在 `forward_list` 中存在时，说明之前已经处理过某条 `load`指令，更新了某个已在`defined_list` 里的变量的值，并把新值加入了 `forward_list`。这时进行替换，把此处 `store` 指令中的值更新成了最新值（常量，或其他变量），这样一来，被删除的 `forward_list` 中的指令的左值都不会再被其他的指令所引用了。

> 在基本块内前进时，`defined_list`代表什么含义？

`defined_list` 是 `Value *` 到 `Instruction *` 的映射，遍历基本块时，对于每一条 `store` 指令，有如下代码：

```
if(defined_list.find(lvalue) != defined_list.end()){
    auto pair = defined_list.find(lvalue);
    delete_list.insert(pair->second);
    pair->second = inst;
}
else{
	defined_list.insert({lvalue, inst});
}
```

它记录了当前基本块中对某个 `lvalue` 进行定值的最新一条指令。

> 生成phi指令`genPhi`的第一步两层for循环在收集什么信息，这些信息在后面的循环中如何被利用生成Phi指令？

外层循环遍历当前函数每个基本块，内层循环遍历基本块中的每条指令，两层循环收集了当前函数中的全局名字集合（即 `std::set<Value *> globals`）以及在基本块中进行定值的“变量-基本块集合”（即 `std::map<Value *, std::set<BasicBlock *>` ）。后者正是在参考文档的9.3.3.2节中提到的稀疏集结构，在插入$\phi$函数时充当初始化的WorkList，对其中每个基本块，在其支配边界基本块的起始处对应插入$\phi$ 函数。

> `valueDefineCounting`为`defined_var`记录了什么信息

对于每个全局名字，有一个对应的计数器和一个栈，`valueDefineCounting` 将$\phi$指令和 `store` 均视为对变量的定值，对应`lvalue` （即当前SSA名字）均加入`defined_var`中该基本块对应的集合。

> `valueForwarding`在遍历基本块时采用的什么方式

递归调用。正如前文所述：遍历当前基本块的所有后继基本块，处理$\phi$指令，填入相应的操作数。然后重复遍历，对其余的 `load` 指令进行重写，对 `store` 进行`value_status`的更新。之后，继续对其在支配树上的每一个后继基本块递归调用`valueForwarding`，完成块间优化。

> `valueForwarding`中为什么`value_status`需要对phi指令做信息收集

该部分相当于对$\phi$指令涉及到的变量做名字重写（以及相应的进栈环节），如果不收集这些信息，则如果某个基本块中某个变量的值可能来自不同分支里的不同基本块，则$\phi$指令后整合产生的新名字在后续就无法被优化处理了。

> `valueForwarding`中第二个循环对`load`指令的替换是什么含义

```
if(inst->get_instr_type() == Instruction::OpID::load){
	Value* lvalue = static_cast<LoadInst *>(inst)->get_lval();
    Value* new_value = *(value_status.find(lvalue)->second.end() - 1);
    inst->replace_all_use_with(new_value);
}
```

对该条指令`load` 的目标变量 `lvalue` ，找到当前`value_status` 栈顶值（即执行到这条指令时目标变量实际的值）`new_value`，并用`new_value`对该 `lvalue` 的所有引用（遍历`use_list_`）进行替换。

> `valueForwarding`中出现的`defined_var`和`value_status`插入条目之间有什么联系

`defined_var` 由基本块映射到该基本块 `define` 过的所有SSA变量名，`value_status`是每一个全局名字映射到SSA形式的变量栈。

```
auto var_set = define_var.find(bb)->second;
for(auto var: var_set){
	if(value_status.find(var) == value_status.end())continue;
    if(value_status.find(var)->second.size() == 0)continue;
    value_status.find(var)->second.pop_back();
}
```

在 `valueForwarding()` 函数中最后作了上述处理。即对某个基本块，遍历它对应的`define_var`集合中的所有变量，在最后使用该记录对`value_status`作出栈处理。由此可以得知，在处理完某一个基本块中的指令时，`define_var`中的条目正好是`value_status`中的栈顶条目。

#### B2-3
**

```
int main(){
    int a, b;
    a = 1;
    b = 1;
    while (a <= 10) {
        if (a % 2 == 0) {
            b = b + a;
            a = a + 1;
        }
        else {
            b = b - a;
            a = a + 3;
        }
    }
    putint(b);
	return 0;
}
```

首先，在 `if` 分支中，均有对 `a, b` 两个变量的定值和引用，否则，则不会生成$\phi$指令。其次，双层嵌套时生成的$\phi$指令共有4条：

```
  label4:                                                ; preds = %label_entry, %label29
  %op30 = phi i32 [ 1, %label_entry ], [ %op32, %label29 ]
  %op31 = phi i32 [ 1, %label_entry ], [ %op33, %label29 ]
  label29:                                                ; preds = %label17, %label23
  %op32 = phi i32 [ %op26, %label23 ], [ %op20, %label17 ]
  %op33 = phi i32 [ %op28, %label23 ], [ %op22, %label17 ]
```

即，至少涉及支配树上的三层，比如

```
1, label_entry	---------------------┓
op20, label17 --┓			  	    |--> op30, label4
		 	   |--->op32, label29 --┙
op26, label23 --┙
```

即能够反映各条$\phi$指令之间的依赖关系。如果单层嵌套，则不能体现支配树的正确与否。

### 活跃变量分析

#### B3-1：活跃变量分析算法设计思路

整体思路借鉴了书中算法9.7，在实现时做出了如下的改动：

* def集合并不是先计算好的，而是在计算IN和OUT的同时计算的
* 书中算法先计算OUT再计算IN，这会给“集合相减”很大的困扰。因此我们先计算IN，同时就可以计算def集合
* 对于phi指令，使用一个map来维护其活跃性来源，以确保phi的正确分析。

下面对实现思路进行详细的介绍：

1. 使用变量continue_flag判断是否结束循环。这个变量表示，这一次迭代没有任何IN和OUT集合发生变化。

2. 对于函数中的每个BasicBlock，维护集合：in_before（前一次的IN集合）、out_before（前一次的OUT集合）和def_list（该块中遍历到当前指令的def集合）。

3. 先对IN集合进行计算，IN集合的计算公式为：
   $$
   IN[B]=use_B\cup(OUT[B]-def_B)
   $$
   这里由于我们先计算了IN，这时OUT还没有被计算，但那没有关系，无非是多迭代一次就好了。

   * 根据operand的type来判断是否是可能需要插入IN集合的变量。如果是，那么应当再def_list中查找（因为def_list记载了出现这个变量时之前所有块内语句是否对其出现了定值操作。），如果没有查找到，证明这是一个属于use集合的变量，应当将其加入IN集合。以此类推，完成了所有use变量的插入。

   * 在这里需要处理phi指令：我们维护了这样的一个数据结构来保存其活跃性来源：

     ```c++
     std::map<BasicBlock *, std::map<Value *, std::set<BasicBlock *>>> active_origin;
     ```
   
     （这里因为需要进行查询，故没有使用效率较高的unordered_map）

     这个数据结构通过所属块和value来索引其活跃性来源的块，对于非phi指令，其活跃性来源可以定义为就来自于本块，而对于phi指令中的操作数，其活跃性来源必须于操作数的下一个位置的bb，即：

     ```c++
     if (inst->is_phi()) active_origin[block][oprand].insert((BasicBlock *)inst->get_operands()[i + 1]);
     ```
   
     （这里的+1就完全体现了“下一个位置的bb”）

   * phi指令是非常特殊的，除了上文中所述的需要记录活跃来源，更需要注意到以下几个特点：

     * phi必须出现在Block的开头，且之前不能有任何非phi指令
     * 单块中phi指令“先用后赋值”，即所有的引用均视为use，而赋值是在所有引用之后进行的（这会对phi的指令相关有影响）
   
     所以，我们维护一个bool变量，用来标识当前是否在一个block的phi指令中。一旦当前指令不是phi，则将其置为false。这个变量的作用是，当向IN集合插入时，如果当前处在phi指令中，则无视def集合直接插入，同时维护好作用域：
   
     ```c++
     if (phi_flag == true || def_list.find(oprand) == def_list.end() ) {
         /* cope with phi inst: op1 relys on bb1, op2 relys on bb2 */
         if (inst->is_phi()) 
             active_origin[block][oprand].insert((BasicBlock *)inst->get_operands()[i + 1]);
         else 
             active_origin[block][oprand].insert(block);
         
         block->get_live_in().insert(oprand);
     }
     ```
   
     这里我们就完成了所有的use的插入，插入一个之后我们就对def_list进行维护：
   
     ```c++
     if(!inst->is_void()) def_list.insert(inst);
     ```
   
   * 最后我们插入OUT-def部分：
   
     ```c++
     for (auto out : block->get_live_out()) {
         if (def_list.find(out) == def_list.end())
             block->get_live_in().insert(out);
     }
     ```
   
   
   
4. 然后我们对OUT集合进行计算：

   书中OUT集合计算公式为：
   $$
   OUT[B] = \cup_{S是B的后继}IN[B]
   $$
   但我们由于有phi指令，我们需要对这个公式进行补充：

   * 对于每一个IN中的变量，我们需要检查其活跃来源。其来源必须要来自于后继或本身：

     ```c++
     if (active_origin[succ_block][succ_in].count(succ_block) || active_origin[succ_block][succ_in].count(block)){
         block->get_live_out().insert(succ_in);
     ```

   *  这种活跃来源，必须有“继承”机制。这是因为，如果一个块中没有对其后继IN集合中某个变量进行赋值或引用的话，按照原公式，依然应该作为其前驱的OUT中的一员，但如此一来，我们上面设计的判断插入方法就出现了问题，所以我们需要再插入OUT集合的时候进行进一步维护：

     ```c++
     if(!def_list.count(succ_in)) 
         active_origin[block][succ_in].insert(block);
     ```

     这样就完成了前驱对后继IN集合中活跃来源的“继承”。

5. 最后对于continue_flag的维护，通过比较两个集合即可：

   ```c++
   if (in_before != block->get_live_in() || out_before != block->get_live_out()) 
       continue_flag = true;
   ```





### 检查器

#### B4-1

1. BasicBlock的前驱后继关系：

   原因：在活跃变量分析中，由于会访问所有后继块，如果处理不当可能会修改这些前驱后继关系，所以进行一次检查。这一次检查也是所有检查的基础——换言之，如果没有这次检查，那么所有的检查都会失去意义。

   检查方法：对每一个块，检查其前驱的后继中是否存在它，再检查其后继的前驱中是否存在它即可：

   ```c++
   for(auto &block : func->get_basic_blocks()){
       for(auto &pre_block : block->get_pre_basic_blocks()){
           auto pre_succ_blocks = pre_block->get_succ_basic_blocks();
           if(find(pre_succ_blocks.begin(), pre_succ_blocks.end(), block) == pre_succ_blocks.end()){
               std::cout << "a block's pre-block's succ-block is not itself!" << std::endl;
               std::cout << "error point: " << block->get_name() <<", pre-block is " << pre_block->get_name() << std::endl;
               exit(0);
           }
       }
       for(auto &succ_block : block->get_succ_basic_blocks()){
           auto succ_pre_blocks = succ_block->get_pre_basic_blocks();
           if(find(succ_pre_blocks.begin(), succ_pre_blocks.end(), block) == succ_pre_blocks.end()){
               std::cout << "a block's succ-block's pre-block is not itself!" << std::endl;
               std::cout << "error point: " << block->get_name() <<", succ-block is " << succ_block->get_name() << std::endl;
               exit(0);
           }
       }
   }
   ```

2. BasicBlock的最后一条语句是否为br或ret：

   原因：这是基本块的基本定义，在转变成SSA格式时，如果处理不当可能会对某些指令造成损害，我们当然不希望基本块的基本定义被损害——那样会导致程序一旦出错，结果是不可控的。此外，在后端LIR pass中，也需要借助这一基本事实来生成代码块。

   检查方法：直接检查最后一条指令即可。

   ```c++
   auto last_inst = block->get_instructions().back();
   if(!last_inst->is_ret() && !last_inst->is_br()){
       std::cout << "last instruction of block is not valid!" << std::endl;
       std::cout << "error point: "<< block->get_name() << ", last instruction is " << last_inst->get_instr_op_name() << std::endl;
       exit(0);
   }
   ```

3. use_def关系：

   原因：在Mem2Reg遍中有对这一部分的引用，而在代码生成时还需要访问use_list表，因此检查一遍是必要的。

   检查方法：直接遍历块内use是否在表中出现即可：

   ```c++
   for(auto inst : block->get_instructions()){
       for(auto oprand : inst->get_operands()){
           auto iter = std::find_if(oprand->get_use_list().begin(), oprand->get_use_list().end(), [inst](Use use){ return use.val_== inst; });
           if(iter == oprand->get_use_list().end()){
               std::cout << "use-def is not valid!" << std::endl;
               std::cout << "error point: " << block->get_name() << ", unvalid inst is " << inst->get_instr_op_name() << 
               ", unvalid operand is " << oprand->get_name() << std::endl;
               exit(0);
           }
       }
       if(!inst->is_void()) {
           def_block[block].insert(inst);
           all_def.insert(inst);
       }
   }
   ```

4. 变量的先定义再引用。

   原因：变量不定义直接引用会直接造成严重的错误。对于本块中使用的变量，要么在使用前有赋值，要么在前驱中有定义。

   检查方法：构建一个本块变量定义表和前驱定义表：

   ```c++
   std::unordered_map<BasicBlock*, std::set<Value*>> def_block;
   std::unordered_map<BasicBlock*, std::set<Value*>> pre_def;
   ```

   之后使用类似于可用表达式分析的方法构建pre_def表，而def_block在前面几次检查中已经顺便构造好：

   ```c++
   while(continue_flag){
       
       continue_flag = false;
       std::set<Value*> def_before;
       for(auto &block : func->get_basic_blocks()){
           def_before.clear();
           def_before.insert(pre_def[block].begin(), pre_def[block].end());
           std::set<Value *>pre_intersection;
           pre_intersection.insert(all_def.begin(), all_def.end());
           pre_def[block].clear();
           for(auto &pre_block : block->get_pre_basic_blocks()){
               std::set<Value*>tmp;
               std::set_union(pre_def[pre_block].begin(), pre_def[pre_block].end(), 
                                   def_block[pre_block].begin(), def_block[pre_block].end(), std::inserter(tmp, tmp.begin()));
               std::set_intersection(pre_intersection.begin(), pre_intersection.end(), 
                                   tmp.begin(), tmp.end(), std::inserter(pre_def[block], pre_def[block].begin()));
               
           }
           if(def_before != pre_def[block]) continue_flag = true;
       }
   }
   ```

   具体公式是：
   $$
   PREDEF[B] = \cup_{S是B的前驱}DEFBLOCK
   $$
   在检查时，对于非phi指令，可以直接访问这个表来检查，而对于phi指令，则需要检查对应前驱块中是否存在定义。检查部分代码较为冗长，具体请参考提交的代码部分。

