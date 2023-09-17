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
    auto module = new Module("if code");  // module name是什么无关紧要
    auto builder = new IRStmtBuilder(nullptr, module);
    Type *Int32Type = Type::get_int32_type(module);

    // int a
    auto a = GlobalVariable::create("a", module, Int32Type, false, CONST_INT(0));

    /* main */
    auto main_fun_ty = FunctionType::get(Int32Type, {});
    auto main_fun = Function::create(main_fun_ty, "main", module);
    auto bb = BasicBlock::create(module, "entry", main_fun);
    builder->set_insert_point(bb);
    builder->create_store(CONST_INT(10), a);
    // return 
    auto retBB = BasicBlock::create(module, "retBB", main_fun); 
    // if branch
    auto trueBB = BasicBlock::create(module, "trueBB_if", main_fun);    
    auto falseBB = BasicBlock::create(module, "falseBB_if", main_fun);  
    // a > 0
    auto a_load = builder->create_load(a);
    auto icmp = builder->create_icmp_gt(a_load, CONST_INT(0));
    builder->create_cond_br(icmp, trueBB, falseBB); 
    // a < 6, flaseBB
    builder->set_insert_point(falseBB);
    icmp = builder->create_icmp_lt(a_load, CONST_INT(6));
    builder->create_cond_br(icmp, trueBB, retBB);
    // trueBB
    builder->set_insert_point(trueBB);
    builder->create_ret(a_load);
    // retBB
    builder->set_insert_point(retBB);
    auto minusa = builder->create_isub(CONST_INT(0), a_load);
    builder->create_ret(minusa);

    std::cout << module->print();
    delete module;
    return 0;
}