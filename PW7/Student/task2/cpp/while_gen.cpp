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

int main() {
    auto module = new Module("while code");  // module name是什么无关紧要
    auto builder = new IRStmtBuilder(nullptr, module);
    Type *Int32Type = Type::get_int32_type(module);
    // int a
    auto a = GlobalVariable::create("a", module, Int32Type, false, CONST_INT(0));
    /* main */
    auto main_fun_ty = FunctionType::get(Int32Type, {});
    auto main_fun = Function::create(main_fun_ty, "main", module);
    auto bb = BasicBlock::create(module, "entry", main_fun);
    builder->set_insert_point(bb);
    // int b
    auto b_alloca = builder->create_alloca(Int32Type);
    // b = 0;
    builder->create_store(CONST_INT(0), b_alloca);
    // a = 3;
    builder->create_store(CONST_INT(3), a);
    // while block
    auto condBB = BasicBlock::create(module, "condBB_while", main_fun);
    auto trueBB = BasicBlock::create(module, "trueBB_while", main_fun);
    auto falseBB = BasicBlock::create(module, "falseBB_while", main_fun);
    auto trueBBif = BasicBlock::create(module, "trueBB_if", main_fun);
    //auto falseBBif = BasicBlock::create(module, "falseBB_if", main_fun);
    builder->create_br(condBB);
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
    // trueBB
    builder->set_insert_point(trueBB);
    a_load = builder->create_load(a);
    b_load = builder->create_load(b_alloca);
    auto iadd = builder->create_iadd(b_load, a_load);
    builder->create_store(iadd, b_alloca);
    auto isub = builder->create_isub(a_load, CONST_INT(1));
    builder->create_store(isub, a);
    builder->create_br(condBB);
    // falseBB
    builder->set_insert_point(falseBB);
    auto ret_alloca = builder->create_alloca(Int32Type);
    builder->create_store(CONST_INT(0), ret_alloca);
    auto ret_load = builder->create_load(ret_alloca);
    builder->create_ret(ret_load);

    std::cout << module->print();
    delete module;
    return 0;
}