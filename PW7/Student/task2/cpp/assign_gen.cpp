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
    auto module = new Module("assign code");
    auto builder = new IRStmtBuilder(nullptr,module);
    Type *Int32Type = Type::get_int32_type(module);
    Type *FType = Type::get_float_type(module);

    auto mainFunTy = FunctionType::get(Int32Type,{});
    auto mainFun = Function::create(mainFunTy, "main", module);

    /* main block */ 
    auto bb = BasicBlock::create(module, "entry", mainFun);
    builder->set_insert_point(bb);
    // return
    auto retAlloca = builder->create_alloca(Int32Type);
    // float b = 1.8
    auto bAlloca = builder->create_alloca(FType);
    builder->create_store(CONST_FP(1.8), bAlloca);
    // int a[2] = {2};
    auto *arrayType_a = ArrayType::get(Int32Type, 2);
    auto aAlloca = builder->create_alloca(arrayType_a);
    auto a0_gep = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(0)});
    builder->create_store(CONST_INT(2), a0_gep);
    // a[1] = a[0] * b
    auto a0_load = builder->create_load(a0_gep);
    auto a0_to_float = builder->create_sitofp(a0_load, FType);
    auto b_load = builder->create_load(bAlloca);
    auto fmul = builder->create_fmul(a0_to_float, b_load);
    auto fmul_to_i = builder->create_fptosi(fmul, Int32Type);
    auto a1_gep = builder->create_gep(aAlloca, {CONST_INT(0), CONST_INT(1)});
    builder->create_store(fmul_to_i, a1_gep);
    // return b
    b_load = builder->create_load(bAlloca);
    builder->create_store(b_load, retAlloca);
    auto ret_load = builder->create_load(retAlloca);
    builder->create_ret(ret_load);
    std::cout << module->print();
    delete module;
    return 0;
}