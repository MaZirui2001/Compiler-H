#include "BasicBlock.h"
#include "Constant.h"
#include "Function.h"
#include "IRStmtBuilder.h"
#include "IRBuilder.h"
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
    ConstantInt::get(num, module_ptr)

#define CONST_FP(num) \
    ConstantFloat::get(num, module_ptr) // 得到常数值的表示,方便后面多次用到

int main() {
    auto builder = new IRBuilder();
    auto module = builder->getModule();
    auto module_ptr = module.get();
    auto scope = builder->getScope();
    auto stmt_builder = builder->getStmtBuilder();
    Type *Int32Type = Type::get_int32_type(module_ptr);
    Type *FType = Type::get_float_type(module_ptr);

    auto getint = scope->find("getint", true);
    auto putint = scope->find("putint", true);
    auto getfarray = scope->find("getfarray", true);
    auto putfarray = scope->find("putfarray", true);

    // int main
    auto main_fun_ty = FunctionType::get(Int32Type, {});
    auto main_fun = Function::create(main_fun_ty, "main", module_ptr);
    auto bb = BasicBlock::create(module_ptr, "entry", main_fun);
    stmt_builder->set_insert_point(bb);
    // int a
    auto a_alloca = stmt_builder->create_alloca(Int32Type);
    // int n
    // auto n_alloca = stmt_builder->create_alloca(Int32Type);
    // int a = getint()
    auto int_a = stmt_builder->create_call(getint, {});
    stmt_builder->create_store(int_a, a_alloca);
    // putint(a)
    stmt_builder->create_call(putint, {int_a});
    // float b[10000] = {1, 2};
    auto *arrayType_b = ArrayType::get(FType, 10000);
    auto b_alloca = stmt_builder->create_alloca(arrayType_b);
    auto b0_gep = stmt_builder->create_gep(b_alloca, {CONST_INT(0), CONST_INT(0)});
    stmt_builder->create_store(CONST_FP(1), b0_gep);
    auto b1_gep = stmt_builder->create_gep(b_alloca, {CONST_INT(0), CONST_INT(1)});
    stmt_builder->create_store(CONST_FP(2), b1_gep);
    // n = getfarray(b)
    auto b = stmt_builder->create_gep(b_alloca, {CONST_INT(0), CONST_INT(0)});
    auto n_load = stmt_builder->create_call(getfarray, {b});
    // stmt_builder->create_store(n_load, n_alloca);
    auto n_plus1 = stmt_builder->create_iadd(n_load, CONST_INT(1));
    stmt_builder->create_call(putfarray, {n_plus1, b});
    // return b[0]
    auto b0_load = stmt_builder->create_load(b0_gep);
    auto b0_int = stmt_builder->create_fptosi(b0_load, Int32Type);
    stmt_builder->create_ret(b0_int);

    std::cout << module_ptr->print();
    delete builder;
    return 0;
}