#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRStmtBuilder.h"
#include "Module.h"
#include "Type.h"

#include <iostream>
#include <memory>

#ifdef DEBUG  // 用于调试信息,大家可以在编译过程中通过" -DDEBUG"来开启这一选项
#define DEBUG_OUTPUT std::cout << __LINE__ << std::endl;  // 输出行号的简单示例
#else
#define DEBUG_OUTPUT
#endif

#define CONST_INT(num) \
    ConstantInt::get(num, module)

#define CONST_FP(num) \
    ConstantFloat::get(num, module) // 得到常数值的表示,方便后面多次用到

int main(){
    auto module = new Module("func code");  // module name是什么无关紧要
    auto builder = new IRStmtBuilder(nullptr, module);
    Type *Int32Type = Type::get_int32_type(module);

    /* add */
    std::vector<Type *> Ints(2, Int32Type);
    auto add_fun_ty = FunctionType::get(Int32Type, Ints);
    auto add_fun = Function::create(add_fun_ty, "add", module);
    /* add block */
    auto bb = BasicBlock::create(module, "entry", add_fun);
    builder->set_insert_point(bb);
    // int ret, a, b
    auto ret_alloca = builder->create_alloca(Int32Type);
    auto a_alloca = builder->create_alloca(Int32Type);
    auto b_alloca = builder->create_alloca(Int32Type);
    // get args
    std::vector<Value *> args;  
    for (auto arg = add_fun->arg_begin(); arg != add_fun->arg_end(); arg++) {
        args.push_back(*arg);
    }
    builder->create_store(args[0], a_alloca);
    builder->create_store(args[1], b_alloca);
    // temp = a + b
    auto a_load = builder->create_load(a_alloca);
    auto b_load = builder->create_load(b_alloca);
    auto iadd = builder->create_iadd(a_load, b_load);
    builder->create_store(iadd, ret_alloca);
    auto ret_load = builder->create_load(ret_alloca);
    builder->create_ret(ret_load);
    

    /* main */
    auto main_fun_ty = FunctionType::get(Int32Type, {});
    auto main_fun = Function::create(main_fun_ty, "main", module);
    bb = BasicBlock::create(module, "entry", main_fun);
    builder->set_insert_point(bb);
    ret_alloca = builder->create_alloca(Int32Type);
    // int a;
    a_alloca = builder->create_alloca(Int32Type);
    // int c;
    auto c_alloca = builder->create_alloca(Int32Type);
    // a = 3
    builder->create_store(CONST_INT(3), a_alloca);
    // c = 5
    builder->create_store(CONST_INT(5), c_alloca);
    // add(a, a+a)
    a_load = builder->create_load(a_alloca);
    iadd = builder->create_iadd(a_load, a_load);
    auto add = builder->create_call(add_fun, {a_load, iadd});
    // c + add(a, a+a)
    auto c_load = builder->create_load(c_alloca);
    iadd = builder->create_iadd(c_load, add);
    // return
    builder->create_store(iadd, ret_alloca);
    ret_load = builder->create_load(ret_alloca);
    builder->create_ret(ret_load);

    std::cout << module->print();
    delete module;
    return 0;

}
